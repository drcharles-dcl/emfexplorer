/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#if !defined(AFX_SCDOCREFLOW_H__0EDFCB7C_8ECC_4E12_B311_5B8272A3CBD7__INCLUDED_)
#define AFX_SCDOCREFLOW_H__0EDFCB7C_8ECC_4E12_B311_5B8272A3CBD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SCDocReflow.h : header file
//

#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCWinGUI.h)
#include SC_INC_WINLIB(SCWinThumbs.h)
#include SC_INC_WINLIB(SCImgStatic.h)
#include SC_INC_EMFLIB(SCEMFDoc.h)
#include <vector>

typedef struct tag_SCPageInfo
{
	int	iOrigIndex;
	PSCThumbImage pThumb;
	PSCEMFDocPage pPage;
	CString strOrigName;

	tag_SCPageInfo()
	{
		iOrigIndex = -1;
		pThumb = NULL;
		pPage = NULL;
	}
	~tag_SCPageInfo()
	{
		if (pThumb)
			delete pThumb;
		if (pPage)
		{
			if (iOrigIndex>=0)
				pPage->SCDetachEMF();

			delete pPage;
		}
	}
} SCPageInfo, *PSCPageInfo;

class SCEMFDoc;

/////////////////////////////////////////////////////////////////////////////
// CSCDocReflow dialog

class CSCDocReflow : public CDialog, public I_SCThumbsHolder
{
// Construction
public:
	CSCDocReflow(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSCDocReflow)
	enum { IDD = IDD_DOC_REFLOW };
	CSCImgStatic	m_PictCurPage;
	CListBox	m_LstPages;
	CString	m_strEdPagePos;
	CString	m_strEdURL;
	CString	m_strEdCurFile;
	CString	m_strEdCredit;
	CString	m_strEdComment;
	CString	m_strDocDir;
	BOOL	m_ChkAsDefault;
	//}}AFX_DATA
	CSCWinThumbs	m_PictAllPages;

// Operation
	void SCSetEMFDoc(SCEMFDoc* pEMFDoc) { m_pEMFDoc = pEMFDoc; }
	// I_SCThumbsHolder
	virtual void SCSetThumbsSize(CSize& size);
	virtual PSCThumbImage SCGetThumbImage(int iThumbIdx);
	//

// Overrides
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCDocReflow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HENHMETAFILE SCGetThumbEMF(int iThumbIdx);
	void SCCreateThumbsControl();
	void SCInitControls();
	void SCSelChanged(BOOL bNotifyThumbs=TRUE);
	void SCCleanup();
	void SCEnableControls();
	void SCMoveSelection(int iDestPos, BOOL bAbsolute=FALSE);
	CString SCGetAddReplaceFilename(LPCTSTR lpszFName, UINT& uiFileType, LPCTSTR lpszTitle=NULL);
	void SCCheckRename();
	void SCSetCtlText(UINT idCtl, LPCTSTR lpszText);
	void SCOnValidateCtlChange(int iCtl);

	// Generated message map functions
	//{{AFX_MSG(CSCDocReflow)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLstPages();
	afx_msg void OnDestroy();
	afx_msg void OnBtnTop();
	afx_msg void OnBtnBottom();
	afx_msg void OnBtnLeft();
	afx_msg void OnBtnRight();
	afx_msg void OnBtnUp();
	afx_msg void OnBtnDown();
	virtual void OnOK();
	afx_msg void OnBtnReplace();
	afx_msg void OnBtnRemove();
	afx_msg void OnBtnAdd();
	afx_msg void OnKillfocusEditCurfile();
	afx_msg void OnKillfocusEditCredit();
	afx_msg void OnKillfocusEditUrl();
	afx_msg void OnKillfocusEditComment();
	afx_msg void OnKillfocusEditPagePos();
	afx_msg void OnChkDefault();
	afx_msg void OnBtnValidate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	SC_DECLARE_ENABLE_CONTROL()

private:
	SCEMFDoc*		m_pEMFDoc;
	CSize			m_ThumbsSize;
	UINT			m_uiNbPages;
	int				m_iNbToRename;
	int				m_iDfltCrdPage;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCDOCREFLOW_H__0EDFCB7C_8ECC_4E12_B311_5B8272A3CBD7__INCLUDED_)
