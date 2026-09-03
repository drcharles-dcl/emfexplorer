/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFViewHelpers.h"
#include "SCEMFViewDefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define SC_COLORSCHEME_ADDCOLOR(colorUsage) \
	ulColorScheme &= ~SC_CSM_##colorUsage##_MASK; \
	switch (iColorStyle) \
	{ \
	case SC_COLOR_TRANSPARENT: \
		ulColorScheme |= SC_CSM_##colorUsage##_TRANSPARENT; \
		break; \
		\
	case SC_COLOR_SYSINDEX: \
		ulColorScheme |= SC_CSM_##colorUsage##_SYSINDEX; \
		crColor = SC_MAKE_SYSCOLOR(crColor); \
		break; \
		 \
	case SC_COLOR_RGBVALUE: \
	default: \
		ulColorScheme |= SC_CSM_##colorUsage##_RGBVALUE; \
		break; \
	}

void SCSchemeAdd_CtlColor(ULONG& ulColorScheme, COLORREF& crColor, int iColorStyle)
{
	SC_COLORSCHEME_ADDCOLOR(CTLCOLOR)
}

void SCSchemeAdd_PaperColor(ULONG& ulColorScheme, COLORREF& crColor, int iColorStyle)
{
	SC_COLORSCHEME_ADDCOLOR(PAPERCOLOR)
}

/////////////////////////////////////////////////////////////////////////////

#define SC_COLORSCHEME_ADDBORDER(BorderShadow) \
	ulColorScheme &= ~SC_CSM_##BorderShadow##_MASK; \
	ulColorScheme |= (bVisible) ? SC_CSM_##BorderShadow##_YES: SC_CSM_##BorderShadow##_NONE;


void SCSchemeAdd_Border(ULONG& ulColorScheme, BOOL bVisible)
{
	SC_COLORSCHEME_ADDBORDER(BORDER)
}

void SCSchemeAdd_Shadow(ULONG& ulColorScheme, BOOL bVisible)
{
	SC_COLORSCHEME_ADDBORDER(SHADOW)
}

