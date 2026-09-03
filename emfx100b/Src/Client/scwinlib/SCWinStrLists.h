/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCWINSTRLISTS_H_
#define _SCWINSTRLISTS_H_

/////////////////////////////////////////////////////////////////////////////
// Types and general functions

typedef struct tag_SCStrEnumAttribute
{
	UINT	sea_data; 
	LPTSTR	sea_str;
} SCStrEnumAttribute, *PSCStrEnumAttribute;
typedef const SCStrEnumAttribute* LPCSCStrEnumAttribute;

/////////////////////////////////////////////////////////////////////////////
// Helper macros

#define SC_DECLARE_OLESTRINGS(prop) \
BOOL SCGet##prop##DisplayString(CString& strValue, UINT uiValue); \
BOOL SCGet##prop##Strings(CStringArray* pStringArray, CDWordArray* pCookieArray);\
void SCFill##prop##Combo(CComboBox* pCombo);\
BOOL SCGet##prop##PredefinedValue(DWORD& dwValue, DWORD dwCookie);\
BOOL SCGet##prop##PredefinedCookie(DWORD& dwCookie, DWORD dwValue);\

/////////////////////////////////////////////////////////////////////////////
// Helper macros

#define SC_FILL_COMBO(cmbAttrs) \
	SCFillCombo(pCombo, (LPCSCStrEnumAttribute)&cmbAttrs, sizeof(cmbAttrs)/sizeof(SCStrEnumAttribute))

#define SC_IMPLEMENT_OLESTRINGS(prop) \
BOOL SCGet##prop##DisplayString(CString& strValue, UINT uiValue) \
{\
	return SCGetGDIpDisplayString(strValue, uiValue,\
		s_##prop, (sizeof(s_##prop)/sizeof(SCStrEnumAttribute)));\
}\
\
BOOL SCGet##prop##Strings(CStringArray* pStringArray, CDWordArray* pCookieArray)\
{\
	return SCGetGDIpPredefinedStrings(pStringArray, pCookieArray,\
		s_##prop, (sizeof(s_##prop)/sizeof(SCStrEnumAttribute)));\
}\
\
void SCFill##prop##Combo(CComboBox* pCombo)\
{\
	SC_FILL_COMBO(s_##prop);\
}\
\
BOOL SCGet##prop##PredefinedValue(DWORD& dwValue, DWORD dwCookie)\
{\
	return SCGetPredefinedValue(dwValue, dwCookie,\
		s_##prop, (sizeof(s_##prop)/sizeof(SCStrEnumAttribute)));\
}\
\
BOOL SCGet##prop##PredefinedCookie(DWORD& dwCookie, DWORD dwValue)\
{\
	return SCGetPredefinedCookie(dwCookie, dwValue,\
		s_##prop, (sizeof(s_##prop)/sizeof(SCStrEnumAttribute)));\
}\

/////////////////////////////////////////////////////////////////////////////
// Enumerated property lists for Ole containers

BOOL SCGetGDIpDisplayString(CString& strValue, UINT uiValue, LPCSCStrEnumAttribute pAttrs, int cAttrs);
BOOL SCGetGDIpPredefinedStrings(CStringArray* pStringArray, CDWordArray* pCookieArray, LPCSCStrEnumAttribute pAttrs, int cAttrs);
BOOL SCGetPredefinedValue(DWORD& dwValue, DWORD dwCookie, LPCSCStrEnumAttribute pAttrs, int cAttrs);
BOOL SCGetPredefinedValue(DWORD& dwValue, DWORD dwCookie, LPCSCStrEnumAttribute pAttrs, int cAttrs);
BOOL SCGetPredefinedCookie(DWORD& dwCookie, DWORD dwValue, LPCSCStrEnumAttribute pAttrs, int cAttrs);


/////////////////////////////////////////////////////////////////////////////
// Comboboxes filling

void SCFillCombo(CComboBox* pCombo, LPCSCStrEnumAttribute pGDIpAttr, DWORD dwCount);
BOOL SCSetComboCurSelFromValue(CComboBox* pCombo, UINT uiValue);


#endif //_SCWINSTRLISTS_H_
//  ------------------------------------------------------------
