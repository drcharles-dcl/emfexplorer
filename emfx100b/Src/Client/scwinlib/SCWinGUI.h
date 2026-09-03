/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCWINGUI_H_
#define _SCWINGUI_H_

//	#define SC_EXTERN_WINPLACEMENT	// do it at module level
extern TCHAR BASED_CODE s_szAppSection[];
extern TCHAR s_szAppPlacementFormat[];

void SCYieldForAWhile();
BOOL SCReadWindowPlacement(LPWINDOWPLACEMENT pwp, LPCTSTR lpszEntry);
void SCWriteWindowPlacement(LPWINDOWPLACEMENT pwp, LPCTSTR lpszEntry);
void SCInitialShowWindow(HWND hWnd, UINT nCmdShow, LPCTSTR lpszEntry);
void SCSaveWindowPlacement(HWND hWnd, LPCTSTR lpszEntry);

/////////////////////////////////////////////////////////////////////
#define SC_DECLARE_ENABLE_CONTROL() \
	void SCEnableControl(UINT uCtl, BOOL bEnable=TRUE); \


#define SC_IMPLEMENT_ENABLE_CONTROL(cls) \
void cls##::SCEnableControl(UINT uCtl, BOOL bEnable/*=TRUE*/) \
{\
	CWnd *pWnd = GetDlgItem(uCtl);\
	if (pWnd)\
		pWnd->EnableWindow(bEnable);\
} \

/////////////////////////////////////////////////////////////////////




#endif //_SCWINGUI_H_
//  ------------------------------------------------------------
