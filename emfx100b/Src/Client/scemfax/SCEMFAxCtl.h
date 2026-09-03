/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#if !defined(AFX_SCEMFAXCTL_H__BE679AC4_EB24_4E27_B8D8_E1674C7F87F5__INCLUDED_)
#define AFX_SCEMFAXCTL_H__BE679AC4_EB24_4E27_B8D8_E1674C7F87F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SCEMFAxCtl.h : Declaration of the CSCEMFAxCtrl ActiveX Control class.

#include "SCGenInclude.h"
#include SC_INC_EMFLIB(SCEMFImage.h)
#include SC_INC_EMFLIB(SCEMFDoc.h)
#include "SCEMFDataPProp.h"

// File types specific to this control
#define SC_EMFAX_FTYPE_EMFONLY	0
#define SC_EMFAX_FTYPE_EMFPLUS	1
#define SC_EMFAX_FTYPE_EMFDUAL	2
#define SC_EMFAX_FTYPE_WMF		3
#define SC_EMFAX_FTYPE_JPG		4
#define SC_EMFAX_FTYPE_PNG		5
#define SC_EMFAX_FTYPE_BMP		6
#define SC_EMFAX_FTYPE_GIF		7

// Error codes specific to this control
#define SC_EMFAX_NOERR			0
#define SC_EMFAX_ERR_INTERNAL	1
#define SC_EMFAX_ERR_OUTOFMEM	2
#define SC_EMFAX_ERR_BADFTYPE	3
#define SC_EMFAX_ERR_BADPATH	4
#define SC_EMFAX_ERR_FWRITE		5
#define SC_EMFAX_ERR_FREAD		6
#define SC_EMFAX_ERR_GDIPLUS	7
#define SC_EMFAX_ERR_WINDOWS	8

// Zzzz!... let it sleep
//	#define SC_FILTERING_PPAGES

/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxCtrl : See SCEMFAxCtl.cpp for implementation.

class CSCEMFAxCtrl : public COleControl, public I_EMFImageOwner
{
	DECLARE_DYNCREATE(CSCEMFAxCtrl)

// Constructor
public:
	CSCEMFAxCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCEMFAxCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual BOOL OnGetPredefinedStrings(DISPID dispid, CStringArray* pStringArray, CDWordArray* pCookieArray);
	virtual BOOL OnGetPredefinedValue(DISPID dispid, DWORD dwCookie, VARIANT* lpvarOut);
	virtual BOOL OnGetDisplayString(DISPID dispid, CString& strValue);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CSCEMFAxCtrl();

	// Subclassed control support
	//BOOL PreCreateWindow(CREATESTRUCT& cs);
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_OLECREATE_EX(CSCEMFAxCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CSCEMFAxCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CSCEMFAxCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CSCEMFAxCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CSCEMFAxCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CSCEMFAxCtrl)
	afx_msg BSTR Get_Picture();
	afx_msg void Set_Picture(LPCTSTR lpszNewValue);
	afx_msg short GetCtlBorder();
	afx_msg void SetCtlBorder(short nNewValue);
	afx_msg short GetCtlColorStyle();
	afx_msg void SetCtlColorStyle(short nNewValue);
	afx_msg OLE_COLOR GetCtlColor();
	afx_msg void SetCtlColor(OLE_COLOR nNewValue);
	afx_msg BOOL GetPageBorderVisible();
	afx_msg void SetPageBorderVisible(BOOL bNewValue);
	afx_msg BOOL GetPageShadowVisible();
	afx_msg void SetPageShadowVisible(BOOL bNewValue);
	afx_msg short GetPaperColorStyle();
	afx_msg void SetPaperColorStyle(short nNewValue);
	afx_msg OLE_COLOR GetPaperColor();
	afx_msg void SetPaperColor(OLE_COLOR nNewValue);
	afx_msg short GetFitMode();
	afx_msg void SetFitMode(short nNewValue);
	afx_msg float GetScale();
	afx_msg void SetScale(float newValue);
	afx_msg OLE_XSIZE_PIXELS GetMarginL();
	afx_msg void SetMarginL(OLE_XSIZE_PIXELS nNewValue);
	afx_msg OLE_YSIZE_PIXELS GetMarginT();
	afx_msg void SetMarginT(OLE_YSIZE_PIXELS nNewValue);
	afx_msg OLE_XSIZE_PIXELS GetMarginR();
	afx_msg void SetMarginR(OLE_XSIZE_PIXELS nNewValue);
	afx_msg OLE_YSIZE_PIXELS GetMarginB();
	afx_msg void SetMarginB(OLE_YSIZE_PIXELS nNewValue);
	afx_msg short GetPageOrientation();
	afx_msg void SetPageOrientation(short nNewValue);
	afx_msg short GetTransparencyMode();
	afx_msg void SetTransparencyMode(short nNewValue);
	afx_msg short GetReverseVideoMode();
	afx_msg void SetReverseVideoMode(short nNewValue);
	afx_msg BOOL GetGDIplusEnabled();
	afx_msg void SetGDIplusEnabled(BOOL bNewValue);
	afx_msg short GetTextRenderingHint();
	afx_msg void SetTextRenderingHint(short nNewValue);
	afx_msg short GetTextContrast();
	afx_msg void SetTextContrast(short nNewValue);
	afx_msg short GetSmoothingMode();
	afx_msg void SetSmoothingMode(short nNewValue);
	afx_msg short GetInterpolationMode();
	afx_msg void SetInterpolationMode(short nNewValue);
	afx_msg short GetPixelOffsetMode();
	afx_msg void SetPixelOffsetMode(short nNewValue);
	afx_msg long SaveAs(LPCTSTR lpszPathName, short nFileType, BOOL bSaveAll);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CSCEMFAxCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CSCEMFAxCtrl)
	dispidPicture = 1L,
	dispidCtlBorder = 2L,
	dispidCtlColorStyle = 3L,
	dispidCtlColor = 4L,
	dispidPageBorderVisible = 5L,
	dispidPageShadowVisible = 6L,
	dispidPaperColorStyle = 7L,
	dispidPaperColor = 8L,
	dispidFitMode = 9L,
	dispidScale = 10L,
	dispidMarginL = 11L,
	dispidMarginT = 12L,
	dispidMarginR = 13L,
	dispidMarginB = 14L,
	dispidPageOrientation = 15L,
	dispidTransparencyMode = 16L,
	dispidReverseVideoMode = 17L,
	dispidGDIplusEnabled = 18L,
	dispidTextRenderingHint = 19L,
	dispidTextContrast = 20L,
	dispidSmoothingMode = 21L,
	dispidInterpolationMode = 22L,
	dispidPixelOffsetMode = 23L,
	dispidSaveAs = 24L,
	//}}AFX_DISP_ID
	};

	// Operation
	void InformDlStatus(CSCEMFDataPathProperty::DLState dlState);

	// I_EMFImageOwner
	virtual void SCSetupCtxMenu(CMenu* pMenu);
	virtual void SCOnCtxCommand(WPARAM wParam, LPARAM lParam);
	virtual CWnd* SCGetMenuOwner();
	//

protected:
	void SCCopyStateToControl();
	void SCShowProperties();
	void SCFileSaveAs(BOOL bSaveAll=FALSE);

	HWND MSGetActualParent();
#ifdef SC_FILTERING_PPAGES
	HRESULT SCGetPropPages(CAUUID *pPPages);
#endif

private:
	// Display
	CSCEMFImage				m_CtlImage;		 // Control to display EMF
	HMENU					m_hPopupMenu;	 // Contextual menu
	// drawing data
	SCEMFDoc				m_EMFDoc;		 // EMF document holder
		// persistent
	CSCEMFDataPathProperty	m_Data;			 // Document loader


	// properties (dup to make life simpler)
	unsigned short			m_usCtlBorder;	 // Border type
	unsigned short			m_usCtlColorStyle;
	OLE_COLOR				m_crCtlColor;
		//--
	BOOL					m_bPageBorderVisible;
	BOOL					m_bPageShadowVisible;
	unsigned short			m_usPaperColorStyle;
	OLE_COLOR				m_crPaperColor;
	unsigned short			m_usFitMode;
	float					m_fScale;
	long					m_lMarginL;
	long					m_lMarginT;
	long					m_lMarginR;
	long					m_lMarginB;
	unsigned short			m_usPageOrientation;
	unsigned short			m_usTransparencyMode;
	unsigned short			m_usReverseVideoMode;
		//--
	BOOL					m_bEnableGDIp;
	unsigned short			m_usTextRenderingHint;
	unsigned short			m_usTextContrast;
	unsigned short			m_usSmoothingMode;
	unsigned short			m_usInterpolationMode;
	unsigned short			m_usPixelOffsetMode;
	//
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCEMFAXCTL_H__BE679AC4_EB24_4E27_B8D8_E1674C7F87F5__INCLUDED)

