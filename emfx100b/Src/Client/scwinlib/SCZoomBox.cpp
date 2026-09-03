/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "SCZoomBox.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define AM_DELAY_UPDATE_NOTIFY	(WM_USER + 1)	// self-posted message

// The caller can use an extended range of zoom values (0..10000 instead of 0..100)
// so that float usage is avoided or limited
// (hence a zoom of 51.2% is represented as 5120)
#define SC_DEFAULT_MULTIPLIER		1 // by default, dont use this technique

#define SC_ZOOMCMB_MAX_DISPLAY_DIGITS		7 // ####.##

#ifdef _UNICODE
	inline double sc_tatof(LPCTSTR lpsz)
	{
		double dValue;
		_stscanf(lpsz, _T("%lf"), &dValue);
		return dValue;
	}
#else
	#define	sc_tatof(d) atof(d)
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCZoomBox

CSCZoomBox::CSCZoomBox():
	m_bTrapEnter(FALSE),
	m_pZoomDefaults(NULL),
	m_usZoomCount(0),
	m_bRestyled(FALSE),
	m_bUserText(FALSE)
{
	SCReset();
}

void CSCZoomBox::SCReset()
{
	m_iFloatMultiplier = SC_DEFAULT_MULTIPLIER;
	m_iMin = 1*SC_DEFAULT_MULTIPLIER;
	m_iMax = 100*SC_DEFAULT_MULTIPLIER;
	m_iValue = 100*SC_DEFAULT_MULTIPLIER;
}

CSCZoomBox::~CSCZoomBox()
{
}

BEGIN_MESSAGE_MAP(CSCZoomBox, CComboBox)
	//{{AFX_MSG_MAP(CSCZoomBox)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillFocus)
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnEditChange)
	ON_MESSAGE( AM_DELAY_UPDATE_NOTIFY, OnSCDelayUpdateAndNotify )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCZoomBox utilities

//
// Multiplier is changing: adjust the current values accordingly.
//
void CSCZoomBox::SCSetFloatMultiplier(int iMultiplier)
{
	ASSERT(iMultiplier>0);
	ASSERT(m_iFloatMultiplier>0);

	m_iMin = MulDiv(m_iMin, iMultiplier, m_iFloatMultiplier);
	m_iMax = MulDiv(m_iMax, iMultiplier, m_iFloatMultiplier);
	m_iValue = MulDiv(m_iValue, iMultiplier, m_iFloatMultiplier);

	m_iFloatMultiplier = iMultiplier;
}

//
// Use the array of zoom entries to fill the combobox if the window exists
//
// Entry values are premultiplied (percentage * FloatMultiplier).
// Since the end-user sees en enters floats, long entries are converted to floats
// except for special (negative) values.
//
BOOL CSCZoomBox::SCFillCombo()
{
	if (!IsWindow(m_hWnd) || !m_pZoomDefaults)
		return FALSE;

	ResetContent();
	CString strZoom;
	for (USHORT i=0; (i<m_usZoomCount); i++)
	{
		int iValue = m_pZoomDefaults[i].ze_iValue;
		ASSERT((iValue<0) || (iValue>=m_iMin && iValue<=m_iMax));
		strZoom = m_pZoomDefaults[i].ze_lpszText;
		if (strZoom.IsEmpty())
		{// default presentation
			strZoom = SCValue2String(iValue);
		}
		int nIndex = AddString(LPCTSTR(strZoom));
		if (nIndex>=0)
		{
			SetItemData(nIndex, (DWORD)iValue);
		}
	}
	SCUpdateEditBox();
	return TRUE;
}

//
// Return the index of iValue if any
//
int CSCZoomBox::SCIndexFromValue(int iValue)
{
	for (USHORT i=0; (i<m_usZoomCount); i++)
	{
		if (iValue == (int)GetItemData(i))
			return (int)i;
	}
	return CB_ERR;
}

//
// Return a visual representation of a percentage
//
CString CSCZoomBox::SCValue2String(int iValue)
{
	ASSERT(m_iFloatMultiplier>0);

	// build the format according to precision: something like "%.2f"
	int iPrecision = (int)log10((float)m_iFloatMultiplier);
	CString strFormat;
	strFormat.Format(_T("%%.%df"), iPrecision);

	CString strValue;
	strValue.Format(strFormat, ((float)iValue)/m_iFloatMultiplier);
	if (iPrecision)
	{// remove trailing zeros after decimal point (2 steps required)
		strValue.TrimRight(_T("0"));
		strValue.TrimRight(_T("."));
	}

	return strValue;
}

//
// Force the current value to be the one passed in argument.
// Values ares expected in the float format but are converted to int.
// (Don't call this for special values)
//
void CSCZoomBox::SCSetCurValue(int iZoom)
{
	// adjust the value
	if (iZoom<m_iMin)
	{
		m_iValue = m_iMin;
	} else
	if (iZoom>m_iMax)
	{
		m_iValue = m_iMax;
	} else
		m_iValue = iZoom;

	// attempt to format an show
	SCUpdateEditBox();
}

//
// Force a value to show in the edit box.
//
void CSCZoomBox::SCSelectValue(int iValue)
{
	for (USHORT i=0; (i<m_usZoomCount); i++)
	{
		if (iValue==m_pZoomDefaults[i].ze_iValue)
		{
			m_iValue = iValue;
			SetCurSel(i);
			break;
		}
	}

	// Make as if someone typed in the value (which must be valid)
	if (iValue>=m_iMin && iValue<=m_iMax)
	{
		m_iValue = iValue;
		SCUpdateEditBox();
	}
}

//
// Display the ext corresponding to m_iValue.
//
inline void CSCZoomBox::SCRestoreValue()
{
	if (m_iValue<0)
		SetCurSel(SCIndexFromValue(m_iValue));
	else
		SCUpdateEditBox();
}

inline BOOL SCIsNum(TCHAR ch)
{
	if ((ch>=_T('0') && ch<=_T('9')) || _T('-')==ch || _T('+')==ch)
		return TRUE;
	return FALSE;
}

//
// Force the edit box's text to become the new value.
//
void CSCZoomBox::SCEatValue()
{
	if (!m_bUserText)
		return;
	m_bUserText = FALSE;

	// Get the value's text
	CString strText;
	GetWindowText(strText);
	strText.TrimLeft(_T(" "));
	
	if (0==strText.GetLength())
	{// restore previous value
		SCRestoreValue();
		return;
	}

	// Compute it
	if (SCIsNum(strText[0]))
	{
		int iValue = (int)(sc_tatof(LPCTSTR(strText))*m_iFloatMultiplier);
		if (iValue==m_iValue)
		{// refresh
			SCUpdateEditBox();
		} else
		{
			// Store new value
			SCSetCurValue(iValue);
			
			// Notify parent
			SCNotifyParent();
		}
	} else
	{
		int iStrIdx = FindString(-1, strText);
		if (iStrIdx!=CB_ERR)
		{// select from text
			SetCurSel(iStrIdx);
			OnSelchange();
		} else
			SCRestoreValue(); // restore previous value
	}
}

//
// Store an array of zoom information
//
void CSCZoomBox::SCSetDefaultList(PSCZoomEntry pZooms, USHORT usCount)
{
	m_pZoomDefaults = pZooms;
	m_usZoomCount = usCount;

	// Update combo, if possible
	SCFillCombo();
}

///
/// Always display percent sign in edit box.
///
void CSCZoomBox::SCUpdateEditBox()
{
	// bail out in case of special value
	if (m_iValue<0)
		return;
	// format an show
	CString strText = SCValue2String(m_iValue) + _T("%");
	SetWindowText(strText);
}

///
/// Tell the parent that selection has changed.
///
void CSCZoomBox::SCNotifyParent()
{
	CWnd *pParent = GetParent();
	
	ASSERT(pParent);
	if (!pParent)
		return;

	NMHDR nmhdr;
	nmhdr.code = CBN_EDITCHANGE;
	nmhdr.hwndFrom = m_hWnd;
	nmhdr.idFrom = GetDlgCtrlID();
	pParent->SendMessage(WM_NOTIFY, (WPARAM)nmhdr.idFrom, (LPARAM)&nmhdr);
}

/////////////////////////////////////////////////////////////////////////////
// CSCZoomBox message handlers

////////////////////////////////////////////////
// Bad, bad, bad land! (though the code is good)
//	
// Warning: it may not be called, so apply settings in dialog template or
// in the call to CreateEx
///
/// Remove sorting attribute before creation.
///
BOOL CSCZoomBox::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CComboBox::PreCreateWindow(cs))
		return FALSE;

	cs.style &= ~CBS_SORT;

	return TRUE;
}

// Warning: A problem in dialog template created classes (CListBox, CEdit, and so on
// created by AppStudio) is that OnCreate is not called.
///
/// Limit text and fill the listbox part with values.
///
int CSCZoomBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SCInitCombo();
	return 0;
}
////////////////////////////////////////////////

// To call in initdialog
BOOL CSCZoomBox::SCInitCombo() 
{
	LimitText(SC_ZOOMCMB_MAX_DISPLAY_DIGITS); // ####.##

	// Fill combobox with default values
	SCFillCombo();
	return TRUE;
}

///
/// Enforcing the ES_WANTRETURN style.
///
HBRUSH CSCZoomBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (!m_bRestyled)
	{
		m_bRestyled = TRUE;
		if (nCtlColor == CTLCOLOR_EDIT)
		{
			// The Edit control
			if (0==(pWnd->GetStyle() & ES_WANTRETURN))
				pWnd->ModifyStyle(0, ES_WANTRETURN);
		}
	}
	return CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
}

////////////////////////////////////////////
/// Bad land! it may not be called in dialog.
///
/// Trap the enter key for special processing.
///
BOOL CSCZoomBox::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		// When the enter key is hit in the ComboBox, we want to get the string
		// and notify the parent.
		if ((pMsg->lParam & 0x40000000) == 0)   // Not a repeat.
		{
			// Get the value
			SCEatValue();
		}
		if (m_bTrapEnter)
			return TRUE;
	}

	return CComboBox::PreTranslateMessage(pMsg);
}
////////////////////////////////////////////

///
/// Called when a new list item is selected.
///
void CSCZoomBox::OnSelchange() 
{
	m_bUserText = FALSE;
	// Validate the selected text
	int iIdx = GetCurSel();
	if (CB_ERR!=iIdx)
	{// actual selection: get the new value from the list
		int iValue = (int)GetItemData(iIdx);
		if (iValue>=0)
		{
			CString strText;
			GetLBText(iIdx, strText);
			// Won't cause notification, but editbox will be erased
			SCSetCurValue((int)(sc_tatof(LPCTSTR(strText))*m_iFloatMultiplier));
		} else
		{// Store special value "as is"
			m_iValue = iValue;
		}
			
		// At this time the edit portion of the combo box has not been updated
		// So we will delay update and parent notification
		PostMessage(AM_DELAY_UPDATE_NOTIFY);
	} else
		SCNotifyParent(); 	// Just notify the parent
}

///
/// Delayed processing of an actual selection.
///
LRESULT CSCZoomBox::OnSCDelayUpdateAndNotify(WPARAM wParam, LPARAM lParam)
{
	// format an show current zoom
	SCUpdateEditBox();

	// Notify parent
	SCNotifyParent();

	return 0L;
}

void CSCZoomBox::OnKillFocus()
{
	// TODO: check text
	SCEatValue();
}

void CSCZoomBox::OnEditChange()
{
	m_bUserText = TRUE;
}

