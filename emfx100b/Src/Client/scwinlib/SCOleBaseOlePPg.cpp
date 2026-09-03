/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCOleBaseOlePPg.h"
#include <afxconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCBaseOlePropertyPage implementation

BEGIN_MESSAGE_MAP(CSCBaseOlePropertyPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CSCBaseOlePropertyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSCBaseOlePropertyPage::CSCBaseOlePropertyPage(UINT idDlg, UINT idCaption) :
	COlePropertyPage(idDlg, idCaption),
	m_lcid(0),
	m_plpDisp(NULL),
	m_ulNbObjects(0)
{
	//{{AFX_DATA_INIT(CSCBaseOlePropertyPage)
	//}}AFX_DATA_INIT
}

CSCBaseOlePropertyPage::~CSCBaseOlePropertyPage()
{
}

IMPLEMENT_DYNAMIC(CSCBaseOlePropertyPage, COlePropertyPage)

void CSCBaseOlePropertyPage::OnObjectsChanged()
{
	m_plpDisp = GetObjectArray(&m_ulNbObjects);
}

/////////////////////////////////////////////////////////////////////////////
// CSCBaseOlePropertyPage data exchange support

template < class cppT, int oleT >
BOOL SCSetAnyProp(cppT cppValue, LPCTSTR pszPropName,
				  LCID lcid, LPDISPATCH* plpDisp, ULONG ulNbObjects)
{
	USES_CONVERSION;
	COleDispatchDriver PropDispDriver;

	for (ULONG i = 0; (i < ulNbObjects); i++)
	{
		DISPID dwDispID;

		// Get the Dispatch ID for the property and if successful set the value
		LPCOLESTR lpOleStr = T2COLE(pszPropName);
		if (SUCCEEDED(plpDisp[i]->GetIDsOfNames(IID_NULL, (LPOLESTR*)&lpOleStr,
			1, lcid, &dwDispID)))
		{
			// Set property
			PropDispDriver.AttachDispatch(plpDisp[i], FALSE);
			try
			{
				PropDispDriver.SetProperty(dwDispID, oleT, cppValue);
			}
			catch(COleDispatchException* pEx)
			{
#ifdef _DEBUG
				// Display message box for dispatch exceptions.
				pEx->ReportError(MB_ICONEXCLAMATION | MB_OK);
#endif
				pEx->Delete();
			}
			PropDispDriver.DetachDispatch();
		}
	}
	return TRUE;
}

#define SC_COMMON_DFLT_PROPVALUE	0 // should be equal to some common default value

template < class cppT, int oleT >
BOOL SCGetAnyProp(cppT* pcppValue, LPCTSTR pszPropName,
				  LCID lcid, LPDISPATCH* plpDisp, ULONG ulNbObjects)
{
	USES_CONVERSION;
	COleDispatchDriver PropDispDriver;

	for (ULONG i = 0; (i < ulNbObjects); i++)
	{
		DISPID dwDispID;

		// Get the Dispatch ID for the property and if successful get the value
		LPCOLESTR lpOleStr = T2COLE(pszPropName);
		if (SUCCEEDED(plpDisp[i]->GetIDsOfNames(IID_NULL, (LPOLESTR*)&lpOleStr,
			1, lcid, &dwDispID)))
		{

			// Get property
			cppT cppValue = SC_COMMON_DFLT_PROPVALUE;

			PropDispDriver.AttachDispatch(plpDisp[i], FALSE);
			try
			{
				PropDispDriver.GetProperty(dwDispID, oleT, &cppValue);
			}
			catch (CException* pEx)
			{
#ifdef _DEBUG
				// Display message box for dispatch exceptions.
				pEx->ReportError(MB_ICONEXCLAMATION | MB_OK);
#endif
				pEx->Delete();
			}
			PropDispDriver.DetachDispatch();

			if (i != 0 && cppValue != *pcppValue)
				*pcppValue = SC_COMMON_DFLT_PROPVALUE;	 // reset
			else
				*pcppValue = cppValue;
		}
	}
	return TRUE;
}

#undef SC_COMMON_DFLT_PROPVALUE


BOOL CSCBaseOlePropertyPage::SCSetDWORDProp(DWORD dwValue, LPCTSTR pszPropName)
{
	return SCSetAnyProp< DWORD, VT_I4 >(dwValue, pszPropName,
		m_lcid, m_plpDisp, m_ulNbObjects);
}

BOOL CSCBaseOlePropertyPage::SCGetDWORDProp(DWORD* pdwValue, LPCTSTR pszPropName)
{
	return SCGetAnyProp< DWORD, VT_I4 >(pdwValue, pszPropName,
		m_lcid, m_plpDisp, m_ulNbObjects);
}

BOOL CSCBaseOlePropertyPage::SCSetFloatProp(float fValue, LPCTSTR pszPropName)
{
	return SCSetAnyProp< float, VT_R4 >(fValue, pszPropName,
		m_lcid, m_plpDisp, m_ulNbObjects);
}

BOOL CSCBaseOlePropertyPage::SCGetFloatProp(float* pfValue, LPCTSTR pszPropName)
{
	return SCGetAnyProp< float, VT_R4 >(pfValue, pszPropName,
		m_lcid, m_plpDisp, m_ulNbObjects);
}


