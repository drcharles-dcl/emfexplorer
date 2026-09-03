/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCPROPHELPERS_H_
#define _SCPROPHELPERS_H_

/////////////////////////////////////////////////////////////////////////////
// Support macros for Ole property pages

/// for enum properties (accessed through cookie)
#define SCDECL_PROP_COOKIE(memberCookie, pprop) \
	int nOld##pprop##Cookie = memberCookie

#define SCDDP_COOKIE_SETPROP(memberCookie, pprop) \
		if (nOld##pprop##Cookie != memberCookie) \
		{\
			DWORD dwPPropValue;\
			if (SCGet##pprop##PredefinedValue(dwPPropValue, memberCookie)) \
				SCSetDWORDProp(dwPPropValue, _T(#pprop)); \
		}

#define SCDDP_COOKIE_GETPROP(memberCookie, pprop) \
	{\
		DWORD dwPPropValue;\
		SCGetDWORDProp(&dwPPropValue, _T(#pprop));\
		DWORD dwCookie;\
		if (SCGet##pprop##PredefinedCookie(dwCookie, dwPPropValue))\
		{\
			memberCookie = dwCookie;\
		}\
	}

/// for other properties (accessed through value)
#define SCDECL_DWORD_PROP(memberValue, pprop) \
	DWORD dwOld##pprop##Value = memberValue

#define SCDDP_SETPROP(memberValue, pprop) \
		if (dwOld##pprop##Value != memberValue) \
		{\
			SCSetDWORDProp(memberValue, _T(#pprop)); \
		}

#define SCDDP_GETPROP(memberValue, pprop) \
	{\
		DWORD dwPPropValue;\
		SCGetDWORDProp(&dwPPropValue, _T(#pprop));\
		memberValue = dwPPropValue;\
	}

#endif //_SCPROPHELPERS_H_
//  ------------------------------------------------------------
