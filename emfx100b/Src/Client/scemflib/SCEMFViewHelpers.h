/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMFVIEWHELPERS_H_
#define _SCEMFVIEWHELPERS_H_

// Color scheme helpers
void SCSchemeAdd_CtlColor(ULONG& ulColorScheme, COLORREF& crColor, int iColorStyle);
void SCSchemeAdd_PaperColor(ULONG& ulColorScheme, COLORREF& crColor, int iColorStyle);

void SCSchemeAdd_Border(ULONG& ulColorScheme, BOOL bVisible);
void SCSchemeAdd_Shadow(ULONG& ulColorScheme, BOOL bVisible);

#endif //_SCEMFVIEWHELPERS_H_
//  ------------------------------------------------------------
