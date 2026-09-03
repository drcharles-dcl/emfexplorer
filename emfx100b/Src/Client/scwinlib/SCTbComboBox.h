/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCTBCOMBOBOX_H_
#define _SCTBCOMBOBOX_H_

/////////////////////////////////////////////////////////////////////////////
// CSCTbComboBox window

class CSCTbComboBox : public CComboBox
{
// Construction / Destruction
public:
	CSCTbComboBox();
	virtual ~CSCTbComboBox();

// Attributes
public:
	// getters

	// setters

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCTbComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CSCTbComboBox)
	afx_msg void OnSelendok();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:

private:
};


#endif //_SCTBCOMBOBOX_H_
//  ------------------------------------------------------------
