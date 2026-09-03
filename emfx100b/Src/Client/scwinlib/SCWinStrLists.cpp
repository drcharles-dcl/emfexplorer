/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCWinStrLists.h"

//	#ifdef _DEBUG
//	#define new DEBUG_NEW
//	#undef THIS_FILE
//	static char THIS_FILE[] = __FILE__;
//	#endif

/////////////////////////////////////////////////////////////////////////////
// Enumerated property lists for Ole containers

BOOL SCGetGDIpDisplayString(CString& strValue, UINT uiValue, LPCSCStrEnumAttribute pAttrs, int cAttrs)
{
	for (int i=0; (i<cAttrs); i++)
	{
		if (pAttrs[i].sea_data==uiValue)
		{
			strValue.Format(_T("%d - %s"), i, pAttrs[i].sea_str);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SCGetGDIpPredefinedStrings(CStringArray* pStringArray, CDWordArray* pCookieArray, LPCSCStrEnumAttribute pAttrs, int cAttrs)
{
	CString strName;
	for (int i=0; (i<cAttrs); i++)
	{
		strName.Format(_T("%d - %s"), i, pAttrs[i].sea_str);
		pStringArray->Add(strName);
		pCookieArray->Add(i);
	}
	return TRUE;
}

BOOL SCGetPredefinedValue(DWORD& dwValue, DWORD dwCookie, LPCSCStrEnumAttribute pAttrs, int cAttrs)
{
	if (dwCookie<(DWORD)cAttrs)
	{
		dwValue = pAttrs[dwCookie].sea_data;
		return TRUE;
	}

	return FALSE;
}

BOOL SCGetPredefinedCookie(DWORD& dwCookie, DWORD dwValue, LPCSCStrEnumAttribute pAttrs, int cAttrs)
{
	for (int i=0; (i<cAttrs); i++)
	{
		// NOTE: pAttrs must not contain duplicates
		if (dwValue == pAttrs[i].sea_data)
		{
			dwCookie = i;
			return TRUE;
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Comboboxes filling

void SCFillCombo(CComboBox* pCombo, LPCSCStrEnumAttribute pGDIpAttr, DWORD dwCount)
{
	ASSERT(pCombo && pGDIpAttr);

	int iIndex;
	for (DWORD i=0; (i<dwCount); i++)
	{
		iIndex = pCombo->AddString(pGDIpAttr[i].sea_str);
		ASSERT(iIndex>=0);
		if (iIndex>=0)
			pCombo->SetItemDataPtr(iIndex, (LPVOID)&pGDIpAttr[i]);
	}
}

BOOL SCSetComboCurSelFromValue(CComboBox* pCombo, UINT uiValue)
{
	ASSERT(pCombo);
	DWORD dwCount = pCombo->GetCount();
	for (DWORD i=0; (i<dwCount); i++)
	{
		PSCStrEnumAttribute pAttr = (PSCStrEnumAttribute)pCombo->GetItemDataPtr(i);
		ASSERT(pAttr);
		if (pAttr->sea_data==uiValue)
		{
			pCombo->SetCurSel(i);
			return TRUE;
		}
	}
	return FALSE;
}

