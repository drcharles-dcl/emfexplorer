/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMFINSPECTDLG_H_
#define _SCEMFINSPECTDLG_H_

#include "SCGenInclude.h"
#include SC_INC_EMFLIB(SCEMF2Text.h)
#include SC_INC_WINLIB(SCWinGUI.h)
#include SC_INC_WINLIB(SCRichEdit.h)
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CSCEMFInspectDlg dialog

class CSCEMFInspectDlg : public CDialog,
					  public I_SCBrkCallBack
{
// Construction
public:
	CSCEMFInspectDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL SCInitDialog();
	virtual ~CSCEMFInspectDlg();

// Dialog Data
	//{{AFX_DATA(CSCEMFInspectDlg)
	enum { IDD = IDD_SCEMFINSPECT_DIALOG };
	CProgressCtrl	m_Progress;
	CString	m_EdBrkCount;
	CString	m_strDPI;
	CString	m_strSize;
	//}}AFX_DATA

//I_BrkCallBack
	virtual long OnSCBrkPos(long lCurPos);
	virtual long OnSCBrkMaxPos(long lMaxPos);
	virtual long OnSCBrkRecStr(long lCurPos, TCHAR *szRecStr);
	
// Operation
	void SCSetEMF(HENHMETAFILE hEmf)
	{
		m_hEmf = hEmf;
		if (IsWindow(m_hWnd))
			SCBeginCracking();
	}
	BOOL SCIsCracking() { return m_bCracking; }
	void SCStopCracking() { OnBtnStopbrk(); }
	void SCBeginCracking();
	BOOL SCSaveDocument(LPCTSTR lpszPathname, BOOL bRTF=FALSE);
	void SCCopy(BOOL bCopyAll=FALSE);
	void SCSelectAll();
	void SCPrint(CDC* pDC, CPrintInfo* pInfo);
	UINT SCCountPages(CDC* pDC, CPrintInfo* pInfo);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCEMFInspectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SCRelocateControls();
	void SCUpdateAllControls();
	void SCUpdateControl(UINT uCtl);
	void SCShowControl(UINT uCtl, int nShow);
	void SCCheckBtnControl(UINT uCtl, BOOL bCheck);

	// Generated message map functions
	//{{AFX_MSG(CSCEMFInspectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBtnStopbrk();
	afx_msg void OnDestroy();
	afx_msg void OnBtnSaveRTF();
	afx_msg void OnBtnCopyAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	SC_DECLARE_ENABLE_CONTROL()

	void SCMarkText(CString sText, COLORREF color);
	void SCCheckTextContent(); 
	void SCEnableGUI(BOOL bStop=TRUE);

protected:
	//data
	CSCRichEdit	m_EdEMFContent;
	CFont		m_Edfont;
	BOOL		m_bStopBrk;
	int			m_nMaxBrkPos;
	BOOL		m_bCracking;
	BOOL		m_bDestroyed;
	HENHMETAFILE m_hEmf;
};

#endif //_SCEMFINSPECTDLG_H_
//  ------------------------------------------------------------
