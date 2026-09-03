/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _WSTREDIT_H_
#define _WSTREDIT_H_

#if _MSC_VER >= 1000 
#pragma once 
#endif // _MSC_VER >= 1000 

#include <afxcmn.h>
#include <afxext.h>

///////////////////////////////////////////////////////////////////////////// 
// CSCRichEdit window 

class CSCRichEdit : public CRichEditCtrl 
{ 
	// Construction 
public: 
	CSCRichEdit(); 
	
	// Attributes 
public: 
	
	// Operations 
public: 
	void OnFocus(BOOL bFocus); 
	void SCToggleItalicText(BOOL bItalic = TRUE); 
	void SCSetTextColor(COLORREF crColor = RGB(255, 255, 255)); 
	void SCToggleBoldText(BOOL bBold = TRUE); 
	void SCAddText(LPCTSTR szTextIn); 
	void SCSetEmpty(); 
	void SCSetFontSize(UINT uiSize);
	void SCSetFont(LPCTSTR lpszFace, UINT uiSize);

	BOOL SCCreateHidden(BOOL bRTF=TRUE);
	BOOL SCLoadFromFile(LPCTSTR lpszFname, BOOL bRTF);
	BOOL SCLoadFromMem(LPBYTE lpByte, DWORD dwLen, BOOL bRTF);
	BOOL SCLoadFromHGlobal(HANDLE hMem, BOOL bRTF);
	
	// Overrides 
	// ClassWizard generated virtual function overrides 
	//{{AFX_VIRTUAL(CSCRichEdit) 
	//}}AFX_VIRTUAL 
	
	// Implementation 
public: 
	virtual ~CSCRichEdit(); 

	// static
	static DWORD CALLBACK SCFileLoad(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb);
	static DWORD CALLBACK SCRTFStreamLoad(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb);
	static DWORD CALLBACK SCTEXTStreamLoad(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb);
	
	// Generated message map functions 
protected: 
	//{{AFX_MSG(CSCRichEdit) 
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct); 
	//}}AFX_MSG 
	
	DECLARE_MESSAGE_MAP() 
}; 

///////////////////////////////////////////////////////////////////////////// 

//{{AFX_INSERT_LOCATION}} 
// Microsoft Developer Studio will insert additional declarations immediately before the previous line. 


#endif //_WSTREDIT_H_
//  ------------------------------------------------------------
