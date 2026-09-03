/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#if !defined(AFX_SCEMFXBASEPROPPAGE_H__CD8EF6E4_FDC8_4872_B5B4_F9A26B6E80C1__INCLUDED_)
#define AFX_SCEMFXBASEPROPPAGE_H__CD8EF6E4_FDC8_4872_B5B4_F9A26B6E80C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SCEMFXBasePropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// I_SCPropExchange
#include "SCGenInclude.h"
#include SC_INC_GENLIB(SCGenDefs.h)
#include SC_INC_WINLIB(SCWinGUI.h)

class  SMC_INTERFACE I_SCPropExchange
{
public:
	// Filter
	virtual BOOL SCWantSettings(int iSender) = 0;

	// Getters
	virtual DWORD SCGetSettings_CtlBorder(int iSender) = 0;
	virtual void SCGetSettings_CtlColor(int iSender, int& iStyle, COLORREF& crColor) = 0;
	
	virtual int SCGetSettings_Scale(int iSender) = 0;
	virtual void SCGetSettings_Margins(int iSender, RECT& rcLTRB) = 0;
	virtual void SCGetSettings_PageBorder(int iSender, BOOL& bBorder, BOOL& bShadow) = 0;
	virtual DWORD SCGetSettings_PageOrientation(int iSender) = 0;
	virtual void SCGetSettings_PaperColor(int iSender, int& iStyle, COLORREF& crColor) = 0;
	virtual DWORD SCGetSettings_TransparencyMode(int iSender) = 0;
	virtual DWORD SCGetSettings_ReverseVideoMode(int iSender) = 0;
	
	virtual DWORD SCGetSettings_GDIplusEnabled(int iSender) = 0;
	virtual void SCGetSettings_TextRendering(int iSender, int& iHint, int& iContrast) = 0;
	virtual void SCGetSettings_Smoothing(int iSender,
		int& iLineSmoothing,
		int& iImgInterpolation,
		int& iImgPixelOffset) = 0;
	
	// Setters
	virtual void SCPutSettings_CtlBorder(int iSender, int iBorder) = 0;
	virtual void SCPutSettings_CtlColor(int iSender, int iStyle, COLORREF crColor) = 0;
	
	virtual void SCPutSettings_Scale(int iSender, int iScale) = 0;
	virtual void SCPutSettings_Margins(int iSender, RECT& rcLTRB) = 0;
	virtual void SCPutSettings_PageBorder(int iSender, BOOL bBorder, BOOL bShadow) = 0;
	virtual void SCPutSettings_PageOrientation(int iSender, int iOrientation) = 0;
	virtual void SCPutSettings_PaperColor(int iSender, int iStyle, COLORREF crColor) = 0;
	virtual void SCPutSettings_TransparencyMode(int iSender, int iMode) = 0;
	virtual void SCPutSettings_ReverseVideoMode(int iSender, int iMode) = 0;
	
	virtual void SCPutSettings_GDIplusEnabled(int iSender, BOOL bEnabled) = 0;
	virtual void SCPutSettings_TextRendering(int iSender, int iHint, int iContrast) = 0;
	virtual void SCPutSettings_Smoothing(int iSender, int iLineSmoothing,
							 int iImgInterpolation,
							 int iImgPixelOffset) = 0;
	
};

/////////////////////////////////////////////////////////////////////////////
// Support macros for non-Ole property pages

/// for enum properties (accessed through cookie)
#define SCPPG_COOKIE_DECL(memberCookie, pprop) \
	int nOld##pprop##Cookie = memberCookie

#define SCPPG_COOKIE_SETPROP(memberCookie, pprop) \
		{\
			DWORD dwPPropValue;\
			if (SCGet##pprop##PredefinedValue(dwPPropValue, memberCookie)) \
				m_pIExchanger->SCPutSettings_##pprop(m_iFolderID, dwPPropValue); \
		}
//				if (nOld##pprop##Cookie != memberCookie)

#define SCPPG_COOKIE_GETPROP(memberCookie, pprop) \
	{\
		DWORD dwPPropValue;\
		dwPPropValue = m_pIExchanger->SCGetSettings_##pprop(m_iFolderID); \
		DWORD dwCookie;\
		if (SCGet##pprop##PredefinedCookie(dwCookie, dwPPropValue))\
			memberCookie = dwCookie;\
	}

/// for other DWORD properties (accessed through value)
#define SCPPG_DWPROP_DECL(memberValue, pprop) \
	DWORD dwOld##pprop##Value = memberValue

#define SCPPG_SETPROP(memberValue, pprop) \
			m_pIExchanger->SCPutSettings_##prop(m_iFolderID, memberValue)
//				if (dwOld##pprop##Value != memberValue)

#define SCPPG_GETPROP(memberValue, pprop) \
		memberValue = m_pIExchanger->SCGetSettings_##prop(m_iFolderID)


// Special, combined, pair: color (value) and its style (cookie)
// Note: the color property gives its name to the combined functions.
#define SCPPG_SETCOLOR(styleMemberCookie, stylePprop, colorMember, colorProp) \
		{\
			DWORD dwStyleValue;\
			if (SCGet##stylePprop##PredefinedValue(dwStyleValue, styleMemberCookie)) \
				m_pIExchanger->SCPutSettings_##colorProp(m_iFolderID, dwStyleValue, colorMember); \
		}
//			if (nOld##stylePprop##Cookie != styleMemberCookie || 
//				dwOld##colorProp##Value != colorMember) 

#define SCPPG_GETCOLOR(styleMemberCookie, stylePprop, colorMember, colorProp) \
		{\
			int iStyleValue;\
			m_pIExchanger->SCGetSettings_##colorProp(m_iFolderID, iStyleValue, colorMember); \
			DWORD dwStyleCookie;\
			if (SCGet##stylePprop##PredefinedCookie(dwStyleCookie, (DWORD)iStyleValue)) \
				styleMemberCookie = dwStyleCookie; \
		}



/////////////////////////////////////////////////////////////////////////////
// CSCEMFXBasePropPage dialog

class CSCEMFXBasePropPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSCEMFXBasePropPage)

// Construction
public:
	CSCEMFXBasePropPage(); // just to compile
	CSCEMFXBasePropPage(UINT nIDTemplate, UINT nIDCaption, I_SCPropExchange* pPx=NULL);
	~CSCEMFXBasePropPage();

// Dialog Data
	//{{AFX_DATA(CSCEMFXBasePropPage)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA
	BOOL SCGetModified() const { return m_bSCModified; }

	void SCSetModified(BOOL bChanged=TRUE)
	{
		m_bSCModified = bChanged;
		SetModified(bChanged);
	}
	void SCSetPropExchanger(I_SCPropExchange* pPx, int iFolder)
	{
		m_pIExchanger = pPx;
		m_iFolderID = iFolder;
	}

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSCEMFXBasePropPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSCEMFXBasePropPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	SC_DECLARE_ENABLE_CONTROL()

protected:
	I_SCPropExchange*	m_pIExchanger;  // object abstraction
	int					m_iFolderID;	// ID of parent to use in exchanges
	BOOL				m_bSCModified;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCEMFXBASEPROPPAGE_H__CD8EF6E4_FDC8_4872_B5B4_F9A26B6E80C1__INCLUDED_)
