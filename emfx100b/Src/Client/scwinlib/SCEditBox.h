/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEDITBOX_H_
#define _SCEDITBOX_H_


/////////////////////////////////////////////////////////////////////////////
// CSCEditBox window

class CSCEditBox : public CEdit
{
// Construction/Destruction
public:
	CSCEditBox();
	virtual ~CSCEditBox();
	void SCReset();

// Attributes
public:
	void SCSetMinMax(int iMin, int iMax) { m_iMin = iMin; m_iMax = iMax; }
	void SCTrapEnterKey(BOOL bTrap=TRUE) { m_bTrapEnter = bTrap; }


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCEditBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CSCEditBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	int		m_iMin;			// minimum
	int		m_iMax;			// and maximum values allowed
	BOOL	m_bTrapEnter;	// if true, trap the 'enter' key
};

/////////////////////////////////////////////////////////////////////////////
#endif //_SCEDITBOX_H_
//  ------------------------------------------------------------
