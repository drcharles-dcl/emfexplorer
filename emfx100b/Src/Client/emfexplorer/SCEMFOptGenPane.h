/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#if !defined(AFX_SCEMFOPTGENPANE_H__8FC73799_8631_4BC3_9DA9_3B0200D75D28__INCLUDED_)
#define AFX_SCEMFOPTGENPANE_H__8FC73799_8631_4BC3_9DA9_3B0200D75D28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SCEMFOptGenPane.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CSCEMFOptGenPane dialog

class CSCEMFOptGenPane : public CPropertyPage
{
// Construction
public:
	CSCEMFOptGenPane();   // standard constructor

	enum SCEDocLoadSettings
	{
		SCE_DLSettingsDefault=0,
		SCE_DLSettingsLast
	};
	
	enum SCEDocSaveSettings
	{
		SCE_DSSettingsConvert=0,
		SCE_DSSettingsWysiwyg
	};

// Dialog Data
	//{{AFX_DATA(CSCEMFOptGenPane)
	enum { IDD = IDD_OPT_GENPANE };
	int			m_iDocLoadSettings;
	int			m_iDocSaveSettings;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCEMFOptGenPane)
	public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SCLoadSettings();

	// Generated message map functions
	//{{AFX_MSG(CSCEMFOptGenPane)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCEMFOPTGENPANE_H__8FC73799_8631_4BC3_9DA9_3B0200D75D28__INCLUDED_)
