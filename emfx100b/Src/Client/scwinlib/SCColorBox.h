/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCCOLORBOX_H_
#define _SCCOLORBOX_H_

#define	SC_COLORITM_INFLATE			3

#define	SC_COLORITM_HEIGHT_RGB		16
#define	SC_COLORITM_HEIGHT_SYS		20

#define	SC_COLORCMB_DROP_HEIGHT_RGB		(32*SC_COLORITM_HEIGHT_RGB)
#define	SC_COLORCMB_DROP_HEIGHT_SYS		(32*SC_COLORITM_HEIGHT_SYS)

// default appropriate style to use
#define SC_COLORCMB_DFLT_STYLE			(CBS_DROPDOWNLIST | CBS_OWNERDRAWVARIABLE)

/////////////////////////////////////////////////////////////////////////////
// CSCColorBox window

class CSCColorBox : public CComboBox
{
// Construction / Destruction
public:
	CSCColorBox();
	virtual void InitColorsComboBox(COLORREF crDefaultColor);

	virtual ~CSCColorBox();
	void SCReset();

// Attributes
public:
	// getters
	COLORREF SCGetCurSelColor() { return m_crSelColor; }

	// setters

// Operations
public:
	virtual COLORREF SCSetCurSelColor(COLORREF lColor);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCColorBox)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpdis);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpmis);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpcis);
	//}}AFX_VIRTUAL

// Implementation
public:
	static LOGPALETTE *SCCreatePaletteEntries();
	static LOGPALETTE *SCCreateVGAPaletteEntries();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSCColorBox)
	afx_msg void OnSelendok();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void DrawEntireItem(LPDRAWITEMSTRUCT lpdis, INT  inflate); 
	void HandleSelectionState(LPDRAWITEMSTRUCT lpdis,INT inflate); 
	void HandleFocusState(LPDRAWITEMSTRUCT lpdis,INT inflate); 

protected:
	virtual COLORREF SCSelectNewColor();
	virtual LOGPALETTE *SCCreatePalette() { return SCCreatePaletteEntries(); }
	int SCComputeFontHeight(); 

	//
	LOGPALETTE *m_pLogPalette;
	BOOL		m_bCustomAllowed;
	COLORREF	m_crSelColor;

private:
	BOOL	 m_bDlgOpened;
	static PALETTEENTRY const s_VgaPalette[];
};

class CSCSysColorBox : public CSCColorBox
{
// Construction / Destruction
public:
	CSCSysColorBox();
	virtual void InitColorsComboBox(COLORREF crDefaultColor);

	virtual ~CSCSysColorBox();

// Attributes
public:
	// getters

	// setters

// Operations
public:
	virtual COLORREF SCSetCurSelColor(COLORREF crColor);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCSysColorBox)
	public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpmis);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpcis);
	//}}AFX_VIRTUAL

// Implementation
public:

protected:
	virtual COLORREF SCSelectNewColor();
	virtual LOGPALETTE *SCCreatePalette() { return SCCreateSysPaletteEntries(); }
	virtual void DrawEntireItem(LPDRAWITEMSTRUCT lpdis, INT  inflate); 

	static LOGPALETTE *SCCreateSysPaletteEntries();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSCSysColorBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
};

/////////////////////////////////////////////////////////////////////////////
#endif //_SCCOLORBOX_H_
//  ------------------------------------------------------------
