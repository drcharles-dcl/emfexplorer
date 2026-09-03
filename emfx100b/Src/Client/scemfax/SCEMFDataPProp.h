/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMFDATAPPROP_H_
#define _SCEMFDATAPPROP_H_

#include "SCGenInclude.h"
#include SC_INC_SHARED(SCZipFile.h)

/////////////////////////////////////////////////////////////////////////////
// CSCEMFDataPathProperty command target

class CSCEMFDataPathProperty : public CDataPathProperty
{
	DECLARE_DYNAMIC(CSCEMFDataPathProperty)
//Constructor
public:
	CSCEMFDataPathProperty(COleControl* pControl = NULL);

// Attributes
public:
	enum DLState
	{
		dlNone,
		dlEMFBody,
		dlDone
	} m_dlState;

	CMemFile		m_CacheFile;
	HENHMETAFILE	m_hEMF;
	DWORD			m_dwArrivedBytes;	// arrived bytes
	DWORD			m_dwRead;			// expanded bytes in cache
	DWORD			m_dwTotal;			// total expected expanded bytes
	DWORD			m_dwPos;			// useable byte
	DWORD			m_dwNeeded;			// needed to complete records
	DWORD			m_dwNbRecs;
	CSCMemZip		m_MemZip;

// Operations
public:
	BOOL SCReadBytes(DWORD dwArriving, LPBYTE pb, DWORD dwLen);
	DWORD SCReadRecords(DWORD dwArriving, LPBYTE& pb);
	HENHMETAFILE SCDetachEMF() { HENHMETAFILE hEMF = m_hEMF; m_hEMF = NULL; return hEMF; }


// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCEMFDataPathProperty)
	public:
	virtual void ResetData();
	protected:
	virtual void OnDataAvailable(DWORD dwSize, DWORD bscfFlag);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CSCEMFDataPathProperty)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
public:
	virtual ~CSCEMFDataPathProperty();

protected:
	DWORD	SCTransferFileContent(CFile* pFrom, CFile* pTo, DWORD dwAmount);
	BOOL	m_bCheckZip;
	BOOL	m_bIsGZ;
};

#endif //_SCEMFDATAPPROP_H_
//  ------------------------------------------------------------
