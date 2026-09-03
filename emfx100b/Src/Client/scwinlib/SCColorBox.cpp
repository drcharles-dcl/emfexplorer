/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCColorBox.h"
#include <afxdlgs.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CSCColorBox

CSCColorBox::CSCColorBox():
	m_pLogPalette(NULL),
	m_bCustomAllowed(TRUE),
	m_bDlgOpened(FALSE),
	m_crSelColor(RGB(255, 255, 255))
{
}

void CSCColorBox::SCReset()
{
	m_crSelColor = RGB(255, 255, 255);
	SetCurSel(-1);
}


CSCColorBox::~CSCColorBox()
{
	if (m_pLogPalette)
		delete m_pLogPalette;
}

BEGIN_MESSAGE_MAP(CSCColorBox, CComboBox)
	//{{AFX_MSG_MAP(CSCColorBox)
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelendok)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelendok)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCColorBox utilities

/////////////////////////////////////////////////////////////////////////////
// (c) Microsoft, 1993 - 1998 / MSDN / OWNCOMBO.C
// (c) Microsoft (adapted)
void CSCColorBox::DrawItem(LPDRAWITEMSTRUCT lpdis) 
{
	if (lpdis->itemID == -1) 
	{ 
	/* We have a request to draw an item in the list box, yet there 
	* are no list box items. This is sent when the user TABS into 
	* an empty list box or an empty list box gets the focus. We 
	* have to indicate (somehow) that this owner-draw list box has 
	* the focus. We do it in response to this message. Note that 
	* lpdis->itemData field would be invalid in this instance so 
	* we can't allow it to fall into our standard routines. 
		*/ 
		HandleFocusState(lpdis, -(int)(SC_COLORITM_INFLATE)); 
	} 
	else 
	{ 
		switch (lpdis->itemAction) 
		{ 
		case ODA_DRAWENTIRE: 
			DrawEntireItem(lpdis, -(int)(SC_COLORITM_INFLATE)); 
			break; 
			
		case ODA_SELECT: 
			HandleSelectionState(lpdis, -((int)(SC_COLORITM_INFLATE) - 2)); 
			break; 
			
		case ODA_FOCUS: 
			HandleFocusState(lpdis, -(int)(SC_COLORITM_INFLATE)); 
			break; 
		} 
	} 
}

// (c) Microsoft
/**************************************************************************** 
 *                                                                          * 
 *  FUNCTION   : HandleSelectionState(LPDRAWITEMSTRUCT, int)                * 
 *                                                                          * 
 *  PURPOSE    : Handles a change in an item selection state. If an item is * 
 *               selected, a black rectangular frame is drawn around that   * 
 *               item; if an item is de-selected, the frame is removed.     * 
 *                                                                          * 
 *  COMMENT    : The black selection frame is slightly larger than the gray * 
 *               focus frame so they won't paint over each other.           * 
 *                                                                          * 
 ****************************************************************************/ 
void  CSCColorBox::HandleSelectionState( 
        LPDRAWITEMSTRUCT        lpdis, 
        INT                     inflate) 
{ 
	RECT    rc; 
	HBRUSH  hbr; 
	
	/* Resize rectangle to place selection frame outside of the focus 
	* frame and the item. 
	*/ 
	CopyRect ((LPRECT)&rc, (LPRECT)&lpdis->rcItem); 
	InflateRect ((LPRECT)&rc, inflate, inflate); 
	
	if (lpdis->itemState & ODS_SELECTED) 
	{ 
		/* selecting item -- paint a black frame */ 
		hbr = (HBRUSH)GetStockObject(BLACK_BRUSH); 
	} 
	else 
	{ 
		/* de-selecting item -- remove frame */ 
		hbr = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_WINDOW)); 
	}
	FrameRect(lpdis->hDC, (LPRECT)&rc, hbr); 
	DeleteObject (hbr); 
} 

// (c) Microsoft
/**************************************************************************** 
 *                                                                          * 
 *  FUNCTION   : HandleFocusState(LPDRAWITEMSTRUCT, int)                    * 
 *                                                                          * 
 *  PURPOSE    : Handle a change in item focus state. If an item gains the  * 
 *               input focus, a gray rectangular frame is drawn around that * 
 *               item; if an item loses the input focus, the gray frame is  * 
 *               removed.                                                   * 
 *                                                                          * 
 *  COMMENT    : The gray focus frame is slightly smaller than the black    * 
 *               selection frame so they won't paint over each other.       * 
 *                                                                          * 
 ****************************************************************************/ 
void CSCColorBox::HandleFocusState( 
        LPDRAWITEMSTRUCT        lpdis, 
        INT                     inflate) 
{ 
	RECT    rc; 
	HBRUSH  hbr; 
	
	/* Resize rectangle to place focus frame between the selection 
	* frame and the item. 
	*/ 
	CopyRect ((LPRECT)&rc, (LPRECT)&lpdis->rcItem); 
	InflateRect ((LPRECT)&rc, inflate, inflate); 
	
	if (lpdis->itemState & ODS_FOCUS) 
	{ 
		/* gaining input focus -- paint a gray frame */ 
		hbr = (HBRUSH)GetStockObject(GRAY_BRUSH); 
	} 
	else 
	{ 
		/* losing input focus -- remove (paint over) frame */ 
		hbr = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_WINDOW)); 
	} 
	FrameRect(lpdis->hDC, (LPRECT)&rc, hbr); 
	DeleteObject (hbr); 
}

// (c) Microsoft (adapted)
/**************************************************************************** 
 *                                                                          * 
 *  FUNCTION   : DrawEntireItem(LPDRAWITEMSTRUCT, int)                      * 
 *                                                                          * 
 *  PURPOSE    : Draws an item and frames it with a selection frame and/or  * 
 *               a focus frame when appropriate.                            * 
 *                                                                          * 
 ****************************************************************************/ 
void CSCColorBox::DrawEntireItem( 
        LPDRAWITEMSTRUCT        lpdis, 
        INT                     inflate) 
{ 
	RECT    rc; 
	HBRUSH  hbr; 

	if (!lpdis || !m_pLogPalette)
		return;
	
	/* Resize rectangle to leave space for frames */ 
	CopyRect ((LPRECT)&rc, (LPRECT)&lpdis->rcItem); 
	InflateRect ((LPRECT)&rc, inflate, inflate); 

	// according to the item index, create the corresponding brush
	// to draw one palette entry
	// get the entry palette corresponding to the index item
	int itemIdx = lpdis->itemData;
	if ((itemIdx>=0) && (itemIdx<m_pLogPalette->palNumEntries))
	{
		COLORREF color = RGB(m_pLogPalette->palPalEntry[itemIdx].peRed,
			m_pLogPalette->palPalEntry[itemIdx].peGreen,
			m_pLogPalette->palPalEntry[itemIdx].peBlue);
		
		// create brush and fill the rectangle
		hbr = (HBRUSH)CreateSolidBrush(color); 
		FillRect(lpdis->hDC, (LPRECT)&rc, hbr); 
		DeleteObject (hbr);
		
		/* Draw or erase appropriate frames */ 
		HandleSelectionState(lpdis, inflate + 4); 
		HandleFocusState(lpdis, inflate + 2); 
		if(lpdis->itemState & ODS_COMBOBOXEDIT)
		{
			m_crSelColor = color;
		}
		return;
	}

	if (m_bCustomAllowed && itemIdx == m_pLogPalette->palNumEntries)
	{
		if(!(lpdis->itemState & ODS_COMBOBOXEDIT))
		{
			::SetBkColor(lpdis->hDC, GetSysColor(COLOR_WINDOW));
			::SetBkMode(lpdis->hDC, OPAQUE);
			
			::SetTextColor(lpdis->hDC, GetSysColor(COLOR_WINDOWTEXT));
			::OffsetRect(&rc, 0, -1); // TODO: remove font's internal leading instead
			::DrawText(lpdis->hDC, _T("Colors..."), 9, (LPRECT)&rc, DT_CENTER|DT_TOP);
		}
		else
		{
			// a sys color can be here
			COLORREF color = m_crSelColor;
			if (0x80000000 & color)
				color = GetSysColor(0x000000FF & color);

			// create brush and fill the rectangle
			hbr = (HBRUSH)CreateSolidBrush(color); 
			FillRect (lpdis->hDC, (LPRECT)&rc, hbr); 
			DeleteObject (hbr);
			
			/* Draw or erase appropriate frames */ 
			HandleSelectionState(lpdis, inflate + 4); 
			HandleFocusState(lpdis, inflate + 2); 
		}
	}
}
// End of (c) 1993 - 1998 Microsoft Corp / MSDN / OWNCOMBO.C
/////////////////////////////////////////////////////////////////////////////

int CSCColorBox::CompareItem(LPCOMPAREITEMSTRUCT lpcis) 
{
	// TODO: Add your message handler code here and/or call default
	
	return 0;
}

// Warning: must use CBS_OWNERDRAWVARIABLE for this to work
void CSCColorBox::MeasureItem(LPMEASUREITEMSTRUCT lpmis) 
{
	// Code to determine the size of specified item
	int iHeight = SC_COLORITM_INFLATE + SCComputeFontHeight() + SC_COLORITM_INFLATE;
	if (iHeight<SC_COLORITM_HEIGHT_RGB)
		lpmis->itemHeight = SC_COLORITM_HEIGHT_RGB;
	else
		lpmis->itemHeight = iHeight;
}

int CSCColorBox::SCComputeFontHeight() 
{
	CDC* pDC = GetDC();
	if (!pDC)
		return -1;

	CFont* pFont = GetFont();
	if (pFont)
		pFont = (CFont*)pDC->SelectObject(pFont);
	else
		pFont = (CFont*)pDC->SelectStockObject(DEFAULT_GUI_FONT);

	CSize szSize = pDC->GetTextExtent(_T("M"), 1);
		
	pDC->SelectObject(pFont);
	ReleaseDC(pDC);

	return szSize.cy;
}

////////////////////////////////////////////////
// Bad, bad, bad land! (though the code is good)
//	
// Warning: it may not be called, so apply settings in dialog template or
// in the call to CreateEx
BOOL CSCColorBox::PreCreateWindow(CREATESTRUCT& cs) 
{
	// Add your specialized code here and/or call the base class
	if (!CComboBox::PreCreateWindow(cs))
		return FALSE;

	cs.style &= ~(CBS_OWNERDRAWFIXED | CBS_SORT);
	cs.style |= CBS_OWNERDRAWVARIABLE;

	return TRUE;
}

// Warning: A problem in dialog template created classes (CListBox, CEdit, and so on
// created by AppStudio) is that OnCreate is not called.
//	int CSCColorBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
//	{
//		if (CComboBox::OnCreate(lpCreateStruct) == -1)
//			return -1;
//	
//		InitColorsComboBox(m_crSelColor);
//		return 0;
//	}
////////////////////////////////////////////////

// This must be called after combo creation in the parent's
// creation/initialization function
void CSCColorBox::InitColorsComboBox(COLORREF crDefaultColor)
{
	m_crSelColor = crDefaultColor;
	if (m_pLogPalette)
		return;

	m_pLogPalette = SCCreatePalette();
	ASSERT(m_pLogPalette);

	// CBS_HASSTRINGS is implied if one of the CBS_OWNERDRAWs is not specified
	BOOL bHasString = (0!=(GetStyle() & CBS_HASSTRINGS));
	ASSERT(!bHasString);
	if (bHasString)
		return; // ModifyStyle won't succeed

	int iNbrOfitems = m_pLogPalette->palNumEntries;
	for (int i=0; (i<iNbrOfitems); i++)
	{
		AddString((LPCTSTR)i);
		// WARNING: Combo does not have the CBS_HASSTRINGS style, so the
		//  normal listbox string is used to store an index to the palette entry
	}
	if (m_bCustomAllowed)
	{ // For custom color
		AddString((LPCTSTR)iNbrOfitems);
		SetCurSel(iNbrOfitems); // Select custom color
	} else
		SetCurSel(CB_ERR);
}

//
// Select the lColor in the combo box
//	the return value is the selected color if it succeeds,
//	-1 if it fails 
//
COLORREF CSCColorBox::SCSetCurSelColor(COLORREF crColor)
{
	// get the corresponding color
	if (!m_pLogPalette)
		return -1;

	m_crSelColor = crColor;
	SetCurSel((m_bCustomAllowed)? m_pLogPalette->palNumEntries : -1);

	return m_crSelColor;
}

void CSCColorBox::OnSelendok() 
{
	COLORREF crNewColor = SCSelectNewColor();

	if (crNewColor == m_crSelColor)
		return;
	m_crSelColor = crNewColor;

	// Notify parent
	CWnd *pParent = GetParent();
	
	ASSERT(pParent);
	NMHDR nmhdr;
	nmhdr.code = CBN_EDITCHANGE;
	nmhdr.hwndFrom = m_hWnd;
	nmhdr.idFrom = GetDlgCtrlID();
	pParent->SendMessage(WM_NOTIFY, (WPARAM)nmhdr.idFrom, (LPARAM)&nmhdr);
}

COLORREF CSCColorBox::SCSelectNewColor()
{
	COLORREF crNewColor;
	int iSelIdx = GetCurSel();
	if (m_bCustomAllowed && iSelIdx == m_pLogPalette->palNumEntries)
	{
		// Avoid opening the dialog twice
		if (m_bDlgOpened)
		{// the combox is just repainting itself after the previous open: do nothing
			m_bDlgOpened = FALSE;
			return m_crSelColor;		
		}

		m_bDlgOpened = TRUE;	// mark that user attempted to choose from dialog
		CColorDialog dlgColor;//(0, CC_ANYCOLOR|CC_RGBINIT);
		if (IDOK != dlgColor.DoModal())
			return m_crSelColor;

		crNewColor = dlgColor.GetColor();
	} else
	{
		PALETTEENTRY* pEntry = &m_pLogPalette->palPalEntry[iSelIdx];
		crNewColor = RGB(pEntry->peRed, pEntry->peGreen, pEntry->peBlue);
	}
	return crNewColor;
}

/////////////////////////////////////////////////////////////////////////////////////////
// CSCColorBox statics
//

LOGPALETTE *CSCColorBox::SCCreatePaletteEntries()
{
	LOGPALETTE *pLgPalette = NULL;

	HDC hDC = ::GetDC(NULL);
	if (!(RC_PALETTE & GetDeviceCaps(hDC,RASTERCAPS))) 
		pLgPalette = SCCreateVGAPaletteEntries();
	else
	{
		// Find out the number of colors on this device.   
		UINT nColors = (1 << (GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES)));  
		
		// allocate memory for palette entries
		PALETTEENTRY *pPaletteEntries = new PALETTEENTRY[nColors];
		
		// get the system palette 
		if (pPaletteEntries && GetSystemPaletteEntries(hDC, 0, nColors, pPaletteEntries))
		{
			// build the logical palette now
			pLgPalette = (LOGPALETTE *) new BYTE[sizeof(LOGPALETTE) + nColors*sizeof(PALETTEENTRY)];
			if (pLgPalette)
			{
				pLgPalette->palVersion = 0x300;
				pLgPalette->palNumEntries = nColors;
				for (UINT i=0;i<nColors;i++)
					pLgPalette->palPalEntry[i] = pPaletteEntries[i];
			}
		}
		delete [] pPaletteEntries;
	}
	::ReleaseDC(NULL, hDC);

	return pLgPalette;
}

PALETTEENTRY const CSCColorBox::s_VgaPalette[] =
{     
	{ 0,   0,   0,    0 },
	{ 0x80,0,   0,    0 }, 
    { 0,   0x80,0,    0 }, 
    { 0x80,0x80,0,    0 }, 
    { 0,   0,   0x80, 0 }, 
    { 0x80,0,   0x80, 0 },  
	{ 0,   0x80,0x80, 0 },
	{ 0x80,0x80,0x80, 0 },
	{ 0xC0,0xC0,0xC0, 0 }, 
    { 0xFF,0,   0,    0 }, 
    { 0,   0xFF,0,    0 }, 
    { 0xFF,0xFF,0,    0 }, 
    { 0,   0,   0xFF, 0 }, 
    { 0xFF,0,   0xFF, 0 }, 
    { 0,   0xFF,0xFF, 0 }, 
    { 0xFF,0xFF,0xFF, 0 } 
}; 

LOGPALETTE *CSCColorBox::SCCreateVGAPaletteEntries()
{
	UINT iNumEntries = sizeof(s_VgaPalette)/sizeof(PALETTEENTRY);
	LOGPALETTE *pLgPalette = (LOGPALETTE *) new BYTE[sizeof(LOGPALETTE) + iNumEntries*sizeof(PALETTEENTRY)];
	if (pLgPalette)
	{
		pLgPalette->palVersion = 0x300;
		pLgPalette->palNumEntries = iNumEntries;
		for (UINT i=0;i<iNumEntries;i++)
			pLgPalette->palPalEntry[i] = s_VgaPalette[i];
	}

	return pLgPalette;
}

/////////////////////////////////////////////////////////////////////////////
// CSCSysColorBox
//

typedef struct tag_SCSysColorsInfo
{
	DWORD     nColorIdx;
	LPCTSTR  lpszColorName;
} SCSysColorsInfo, *PSCSysColorsInfo;

SCSysColorsInfo s_SysColorsList[] =
{
	COLOR_3DDKSHADOW,				_T("Dark shadow of 3D elements"),
	COLOR_3DFACE,					_T("Face color of 3D elements"),
	COLOR_3DHIGHLIGHT,				_T("Highlight color of 3D elements"),
	COLOR_3DLIGHT,					_T("Light color of 3D elements"),
	COLOR_3DSHADOW,					_T("Shadow color of 3D elements"),
	COLOR_ACTIVEBORDER,				_T("Active window border"),
	COLOR_ACTIVECAPTION,			_T("Active window title bar"),
	COLOR_APPWORKSPACE,				_T("Background color of MDI applications"),
	COLOR_BACKGROUND,				_T("Desktop background"),
	COLOR_BTNTEXT,					_T("Text on push buttons"),
	COLOR_CAPTIONTEXT,				_T("Text in caption and scroll bar arrow box"),
	COLOR_GRADIENTACTIVECAPTION,	_T("Right side in active window's title bar"),
	COLOR_GRADIENTINACTIVECAPTION,	_T("Right side in inactive window's title bar"),
	COLOR_GRAYTEXT,					_T("Grayed (disabled) text"),
	COLOR_HIGHLIGHT,				_T("Item selected in a control"),
	COLOR_HIGHLIGHTTEXT,			_T("Text of selected items in a control"),
	COLOR_HOTLIGHT,					_T("Hot-tracked item"),
	COLOR_INACTIVEBORDER,			_T("Inactive window border"),
	COLOR_INACTIVECAPTION,			_T("Inactive window caption"),
	COLOR_INACTIVECAPTIONTEXT,		_T("Text of inactive caption"),
	COLOR_INFOBK,					_T("Background of tooltips"),
	COLOR_INFOTEXT,					_T("Text color of tooltips"),
	COLOR_MENU,						_T("Menu background"),
	COLOR_MENUTEXT,					_T("Text in menus"),
	COLOR_SCROLLBAR,				_T("Scroll bar gray area"),
	COLOR_WINDOW,					_T("Window background"),
	COLOR_WINDOWFRAME,				_T("Window frame"),
	COLOR_WINDOWTEXT,				_T("Text in windows")
};


CSCSysColorBox::CSCSysColorBox()
{
	m_bCustomAllowed = FALSE;
	m_crSelColor = 0x80000000 | COLOR_WINDOW;
}

CSCSysColorBox::~CSCSysColorBox()
{
}

BEGIN_MESSAGE_MAP(CSCSysColorBox, CSCColorBox)
	//{{AFX_MSG_MAP(CSCSysColorBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CSCSysColorBox::InitColorsComboBox(COLORREF crDefaultColor)
{
	CSCColorBox::InitColorsComboBox(crDefaultColor);
	m_crSelColor = 0x80000000 | (crDefaultColor & 0x000000FF);

	// Enlarge combo
	CDC* pDC = GetDC();
	if (!pDC)
		return;

	int iWdt = GetDroppedWidth();
	CFont* pFont = GetFont();
	if (pFont)
		pFont = (CFont*)pDC->SelectObject(pFont);
	else
		pFont = (CFont*)pDC->SelectStockObject(DEFAULT_GUI_FONT);

	UINT iNumEntries = sizeof(s_SysColorsList)/sizeof(SCSysColorsInfo);
	for (UINT i=0; (i<iNumEntries); i++)
	{
		CSize szSize = pDC->GetTextExtent(s_SysColorsList[i].lpszColorName,
									_tcslen(s_SysColorsList[i].lpszColorName));
		
		if (szSize.cx>iWdt)
			iWdt = szSize.cx;
	}
	pDC->SelectObject(pFont);
	ReleaseDC(pDC);

	if (CB_ERR!=iWdt)
		SetDroppedWidth(iWdt);
}


// Warning: must use CBS_OWNERDRAWVARIABLE for this to work
void CSCSysColorBox::MeasureItem(LPMEASUREITEMSTRUCT lpmis) 
{
	// Code to determine the size of specified item
	CSCColorBox::MeasureItem(lpmis);
	if (lpmis->itemHeight<SC_COLORITM_HEIGHT_SYS)
		lpmis->itemHeight = SC_COLORITM_HEIGHT_SYS;
}

int CSCSysColorBox::CompareItem(LPCOMPAREITEMSTRUCT lpcis) 
{
// MFC bug: it can call us for the CB_ERR item
//		if (lpcis->itemID1<0)
//			return -1;
//		if (lpcis->itemID2<0)
//			return 1;
//	

	DWORD dwNumEntries = sizeof(s_SysColorsList)/sizeof(SCSysColorsInfo);
	DWORD itemIdx1 = lpcis->itemData1;
	DWORD itemIdx2 = lpcis->itemData2;

	if (itemIdx1 < 0 ||
		itemIdx1 >= dwNumEntries ||
		itemIdx2 < 0 ||
		itemIdx2 >= dwNumEntries)
		return 0;       // err

	return _tcsicmp(s_SysColorsList[itemIdx1].lpszColorName,
					s_SysColorsList[itemIdx2].lpszColorName);
}

void CSCSysColorBox::DrawEntireItem( 
        LPDRAWITEMSTRUCT        lpdis, 
        INT                     inflate) 
{ 
	RECT    rc; 
	HBRUSH  hbr; 

	if (!lpdis || !m_pLogPalette)
		return;
	
	/* Resize rectangle to leave space for frames */ 
	CopyRect((LPRECT)&rc, (LPRECT)&lpdis->rcItem); 
	InflateRect((LPRECT)&rc, inflate, inflate); 

	// according to the item index, create the corresponding brush
	// to draw one palette entry
	// get the entry palette corresponding to the index item
	int itemIdx = lpdis->itemData;
	if ((itemIdx>=0) && (itemIdx<m_pLogPalette->palNumEntries))
	{
		COLORREF color = RGB(m_pLogPalette->palPalEntry[itemIdx].peRed,
			m_pLogPalette->palPalEntry[itemIdx].peGreen,
			m_pLogPalette->palPalEntry[itemIdx].peBlue);
		HDC hDC = lpdis->hDC;

		// background
		hbr = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_WINDOW)); 
		FillRect(hDC, (LPRECT)&rc, hbr);
		DeleteObject(hbr);

		// create brush and fill the rectangle
		InflateRect((LPRECT)&rc, -2, -2);
		rc.right = rc.left + (rc.bottom - rc.top);
		hbr = (HBRUSH)CreateSolidBrush(color);
		HBRUSH hOldbr = (HBRUSH)SelectObject(hDC, hbr);
		Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
		SelectObject(hDC, hOldbr);
		DeleteObject(hbr);

		// Draw text
		SetBkMode(hDC, TRANSPARENT);
		::SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
		// TODO: remove font's internal leading instead of hard-coded 2
		TextOut(hDC, rc.right + 4, rc.top - 2,
			s_SysColorsList[itemIdx].lpszColorName, _tcslen(s_SysColorsList[itemIdx].lpszColorName));
		
		/* Draw or erase appropriate frames */ 
		HandleSelectionState(lpdis, inflate + 4); 
		HandleFocusState(lpdis, inflate + 2); 
		if(lpdis->itemState & ODS_COMBOBOXEDIT)
		{
			m_crSelColor = 0x80000000 | s_SysColorsList[itemIdx].nColorIdx;
		}
	}
}

COLORREF CSCSysColorBox::SCSelectNewColor()
{
	int iSelIdx = GetCurSel();
	if (CB_ERR != iSelIdx)
	{
		iSelIdx = GetItemData(iSelIdx);
		if (iSelIdx>=0 && iSelIdx < m_pLogPalette->palNumEntries)
		{
			return 0x800000 | s_SysColorsList[iSelIdx].nColorIdx;
		}
	}

	return m_crSelColor;
}

COLORREF CSCSysColorBox::SCSetCurSelColor(COLORREF crColor)
{
	// get the corresponding color
	if (!m_pLogPalette)
		return -1;

	DWORD nColorIndex = (DWORD)(crColor & 0x000000FF);
	m_crSelColor = 0x80000000 | nColorIndex;

	DWORD dwNumEntries = sizeof(s_SysColorsList)/sizeof(SCSysColorsInfo);
	for (DWORD i=0; (i<dwNumEntries); i++)
	{
		DWORD nPalIdx = GetItemData(i);
		if (nPalIdx>=0 && nPalIdx<dwNumEntries &&
			s_SysColorsList[nPalIdx].nColorIdx == nColorIndex)
		{
			SetCurSel(i);
			return m_crSelColor;
		}
	}

	SetCurSel(CB_ERR);
	return m_crSelColor;
}


// static
LOGPALETTE *CSCSysColorBox::SCCreateSysPaletteEntries()
{
	UINT iNumEntries = sizeof(s_SysColorsList)/sizeof(SCSysColorsInfo);
	LOGPALETTE *pLgPalette = (LOGPALETTE *) new BYTE[sizeof(LOGPALETTE) + iNumEntries*sizeof(PALETTEENTRY)];
	if (pLgPalette)
	{
		pLgPalette->palVersion = 0x300;
		pLgPalette->palNumEntries = iNumEntries;
		for (UINT i=0;i<iNumEntries;i++)
		{
			COLORREF color = GetSysColor(s_SysColorsList[i].nColorIdx);
			PALETTEENTRY* ppalentry = &pLgPalette->palPalEntry[i];
			ppalentry->peFlags = 0;
			ppalentry->peRed = GetRValue(color);
			ppalentry->peGreen = GetGValue(color);
			ppalentry->peBlue = GetBValue(color);
			
		}
	}

	return pLgPalette;
}


