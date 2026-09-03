/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCOLEBASEOLEPPG_H_
#define _SCOLEBASEOLEPPG_H_
#include <afxctl.h>         // MFC support for ActiveX Controls

/////////////////////////////////////////////////////////////////////////////
// Support macros
#include "SCPropHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// Base Ole Property Page Class

class CSCBaseOlePropertyPage : public COlePropertyPage
{
	DECLARE_DYNAMIC(CSCBaseOlePropertyPage)

// Constructors/Destructor
public:
	CSCBaseOlePropertyPage(UINT idDlg, UINT idCaption);
	~CSCBaseOlePropertyPage();

// Operations
//		void SCSetLCID(LCID lcid) { m_lcid = lcid; }

// Implementation

// Overridables
	virtual void OnObjectsChanged();

protected:
// VC won't allow us to do it. We must trick.
//		template < class cppT, int oleT >
//			BOOL SCSetAnyProp(cppT cppValue, LPCTSTR pszPropName);
//		template < class cppT, int oleT >
//			BOOL SCGetAnyProp(cppT* pcppValue, LPCTSTR pszPropName);

	BOOL SCSetDWORDProp(DWORD dwValue, LPCTSTR pszPropName);
	BOOL SCGetDWORDProp(DWORD* pdwValue, LPCTSTR pszPropName);
	BOOL SCSetFloatProp(float fValue, LPCTSTR pszPropName);
	BOOL SCGetFloatProp(float* pfValue, LPCTSTR pszPropName);

	// Generated message map functions
	//{{AFX_MSG(CSCBaseOlePropertyPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	LCID			m_lcid;
	LPDISPATCH*		m_plpDisp;
	ULONG			m_ulNbObjects;
};


#endif //_SCOLEBASEOLEPPG_H_
//  ------------------------------------------------------------
