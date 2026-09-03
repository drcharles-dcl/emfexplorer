/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCRichEdit.h"
#include <objidl.h>	// IStream
#include <afxpriv.h> // conversion

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////////////////////// 
// CSCRichEdit 

CSCRichEdit::CSCRichEdit() 
{ 
} 

CSCRichEdit::~CSCRichEdit() 
{ 
} 


BEGIN_MESSAGE_MAP(CSCRichEdit, CRichEditCtrl) 
//{{AFX_MSG_MAP(CSCRichEdit) 
ON_WM_CREATE() 
//}}AFX_MSG_MAP 
END_MESSAGE_MAP() 


void CSCRichEdit::OnFocus(BOOL bFocus) 
{ 
//		if (bFocus) 
//		{ 
//			//we received focus 
//		} 
//		else 
//		{ 
//			//we lost focus 
//		} 
} 


void CSCRichEdit::SCAddText(LPCTSTR szTextIn) 
{ 
	ASSERT(IsWindow(m_hWnd));

	int iTextLength = GetTextLength(); 
	SetSel(iTextLength,iTextLength); 
	ReplaceSel(szTextIn); 
} 

void CSCRichEdit::SCToggleBoldText(BOOL bBold/*=TRUE*/) 
{ 
	ASSERT(IsWindow(m_hWnd));

	CHARFORMAT cf; 
	
	cf.cbSize = sizeof (CHARFORMAT);  
	cf.dwMask = CFM_BOLD; 
	cf.dwEffects = (bBold) ? CFE_BOLD : CFE_AUTOCOLOR; 
	
	SetSelectionCharFormat(cf); 
} 

void CSCRichEdit::SCToggleItalicText(BOOL bItalic/*=TRUE*/) 
{ 
	CHARFORMAT cf; 
	
	cf.cbSize = sizeof (CHARFORMAT);  
	cf.dwMask = CFM_ITALIC; 
	cf.dwEffects = (bItalic) ? CFE_ITALIC : CFE_AUTOCOLOR; 
	
	SetSelectionCharFormat(cf); 
} 

void CSCRichEdit::SCSetTextColor(COLORREF crColor/* = RGB(255, 255, 255)*/) 
{ 
	ASSERT(IsWindow(m_hWnd));

	CHARFORMAT cf; 
	
	cf.cbSize = sizeof(cf);  
	cf.dwMask = CFM_COLOR; 
	cf.crTextColor = crColor; 
	
	SetSelectionCharFormat(cf); 
}

void CSCRichEdit::SCSetFontSize(UINT uiSize) 
{ 
	//set the default font 
	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);  
	cf.dwMask = CFM_SIZE ; 
	cf.yHeight = uiSize; // twips required
	SetDefaultCharFormat(cf); 
}

void CSCRichEdit::SCSetFont(LPCTSTR lpszFace, UINT uiSize) 
{ 
	BOOL bBold = TRUE;
	//set the default font 
	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);  
	cf.dwMask = CFM_SIZE|CFM_FACE|CFM_BOLD; 
	cf.yHeight = uiSize; // twips required
	cf.dwEffects = (bBold) ? CFE_BOLD : CFE_AUTOCOLOR; 

#if (_RICHEDIT_VER >= 0x0200)
	_tcscpy(cf.szFaceName, lpszFace);
#else
	USES_CONVERSION;
	char* szFace = T2A((LPTSTR)lpszFace);
	strcpy(cf.szFaceName, szFace);
	// szFace is on the stack: don't call delete
#endif

	SetDefaultCharFormat(cf); 
}

int CSCRichEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)  
{ 
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1) 
		return -1; 
	
	//set the default font 
	CHARFORMAT cf;

	cf.cbSize = sizeof(cf);  
	cf.dwMask = CFM_FACE ; 
#if (_RICHEDIT_VER >= 0x0200)
	_tcscpy(cf.szFaceName, _T("Arial"));
#else
	strcpy(cf.szFaceName, "Arial");
#endif
	SetDefaultCharFormat(cf); 
	
	return 0; 
} 

void CSCRichEdit::SCSetEmpty()
{
	ASSERT(IsWindow(m_hWnd));

	SetWindowText(_T(""));
	UpdateWindow();
}

///////////////////////////////////////////////////////////////////////////// 
// Loading text
//

BOOL CSCRichEdit::SCLoadFromFile(LPCTSTR lpszFname, BOOL bRTF)
{
	ASSERT(IsWindow(m_hWnd));

	CFile sourceFile;
	CFileException ex;
	if (!sourceFile.Open(lpszFname, CFile::modeRead, &ex))
		return FALSE;

	// Read the text in
	EDITSTREAM es;
	es.dwError = 0;
	es.pfnCallback = CSCRichEdit::SCFileLoad;
	es.dwCookie = (DWORD)&sourceFile;
	StreamIn((bRTF) ? SF_RTF : SF_TEXT, es);
	sourceFile.Close();

	return TRUE;
}

BOOL CSCRichEdit::SCLoadFromMem(LPBYTE lpByte, DWORD dwLen, BOOL bRTF)
{
	ASSERT(IsWindow(m_hWnd));

	CMemFile memFile;
	memFile.Attach(lpByte, dwLen);
	
	// Read the text in
	EDITSTREAM es;
	es.dwError = 0;
	es.pfnCallback = CSCRichEdit::SCFileLoad;
	es.dwCookie = (DWORD)&memFile;
	StreamIn((bRTF) ? SF_RTF : SF_TEXT, es);
	
	memFile.Detach();
	return TRUE;
}

BOOL CSCRichEdit::SCLoadFromHGlobal(HANDLE hMem, BOOL bRTF)
{
	ASSERT(IsWindow(m_hWnd));

	IStream* pIStream = NULL;
	HRESULT hr = ::CreateStreamOnHGlobal((HGLOBAL)hMem, FALSE, &pIStream);
	if ((hr != S_OK) || !pIStream)
	{
		return FALSE;
	}

	// Read the text in
	EDITSTREAM es;
	es.dwError = 0;
	es.pfnCallback = (bRTF) ? CSCRichEdit::SCRTFStreamLoad : CSCRichEdit::SCTEXTStreamLoad;
	es.dwCookie = (DWORD)pIStream;
	StreamIn((bRTF) ? SF_RTF : SF_TEXT|SF_UNICODE, es);

	pIStream->Release();
	return TRUE;
}


///
/// Prepare hidden rich edit control to render text.
///
BOOL CSCRichEdit::SCCreateHidden(BOOL bRTF/*=TRUE*/)
{
	#define SC_HIDDENCTL_ID		65000

	CWnd* pWnd = ::AfxGetMainWnd();
	CRect rcEdit(0, 0, 1, 1);
	DWORD dwStyle = WS_CHILD|ES_MULTILINE|ES_AUTOVSCROLL|ES_WANTRETURN|CS_DBLCLKS;
	BOOL bOK = Create(dwStyle, rcEdit, pWnd, SC_HIDDENCTL_ID);
	ASSERT(bOK);

	// select a font when bRTF is FALSE
	if (bOK && !bRTF && IsWindow(m_hWnd))
	{
		this->SendMessage(WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
	}

	#undef	SC_HIDDENCTL_ID
	return bOK;
}


///////////////////////////////////////////////////////////////////////////// 
// CSCRichEdit static

DWORD CALLBACK CSCRichEdit::SCFileLoad(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
{
	CFile* pFile = (CFile*)dwCookie;
	
	try
	{
		*pcb = pFile->Read(pbBuff, cb);
	}
	catch (CFileException* pEx)
	{
		pEx->Delete();
		return 1; // Failed so stop calling us.
	}
	return 0;
}

DWORD CALLBACK CSCRichEdit::SCRTFStreamLoad(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
{
	HRESULT hr = ((IStream*)dwCookie)->Read(pbBuff, cb, (ULONG*)pcb);
	if (hr != S_OK)
		return 1; // Failed so stop calling us.

	return 0;
}

DWORD CALLBACK CSCRichEdit::SCTEXTStreamLoad(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
{	
	HRESULT hr = ((IStream*)dwCookie)->Read(pbBuff, cb, (ULONG*)pcb);
	if (hr != S_OK)
		return 1; // Failed so stop calling us.

	if ((*pcb < cb) && (*pcb > sizeof(USHORT)))
	{// don't read the trailing \0\0
		// Note: UNICODE specific
		WCHAR* pLastCh = (WCHAR*)(pbBuff + *pcb - sizeof(WCHAR));
		if ((WCHAR)0==*pLastCh)
		{
			*pcb = *pcb - sizeof(WCHAR);
		}
	}
	return 0;
}

