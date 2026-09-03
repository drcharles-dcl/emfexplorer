/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCZOOMBOX_H_
#define _SCZOOMBOX_H_

typedef struct tag_SCZoomEntry
{
	LPCTSTR	ze_lpszText;
	int		ze_iValue;
} SCZoomEntry, *PSCZoomEntry;

/////////////////////////////////////////////////////////////////////////////
// CSCZoomBox window

class CSCZoomBox : public CComboBox
{
// Construction/Destruction
public:
	CSCZoomBox();
	BOOL SCInitCombo();

	virtual ~CSCZoomBox();
	void SCReset();

// Attributes
public:
	int SCGetCurValue() const { return m_iValue; }
	void SCSetCurValue(int iZoom);

// Operations
public:
	void SCSetFloatMultiplier(int iMultiplier);
	void SCSetMinMax(int iMin, int iMax) { m_iMin = iMin; m_iMax = iMax; }
	void SCSetDefaultList(PSCZoomEntry pZooms, USHORT usCount);
	void SCTrapEnterKey(BOOL bTrap=TRUE) { m_bTrapEnter = bTrap; }
	void SCSelectValue(int iValue);
	void SCEatValue();
	int SCIndexFromValue(int iValue);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCZoomBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	// Self-posted messages
	afx_msg LRESULT OnSCDelayUpdateAndNotify(WPARAM wParam, LPARAM lParam);

	// Generated message map functions
protected:
	//{{AFX_MSG(CSCZoomBox)
	afx_msg void OnSelchange();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnKillFocus();
	afx_msg void OnEditChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	CString SCValue2String(int iValue);
	void SCCleanupDefaultList();
	BOOL SCFillCombo();
	void SCUpdateEditBox();
	void SCNotifyParent();
	inline void SCRestoreValue();

private:
	int				m_iValue;			// selected value
	int				m_iMin;				// minimum
	int				m_iMax;				// and maximum values allowed
	PSCZoomEntry	m_pZoomDefaults;	// list of default values
	USHORT			m_usZoomCount;		// count of default values
	int				m_iFloatMultiplier; // multiplier to avoid using floats
	BOOL			m_bTrapEnter;		// if true, trap the 'enter' key
	BOOL			m_bRestyled;		// to enforce some styles
	BOOL			m_bUserText;		// user typed something
};

/////////////////////////////////////////////////////////////////////////////
#endif //_SCZOOMBOX_H_
//  ------------------------------------------------------------
