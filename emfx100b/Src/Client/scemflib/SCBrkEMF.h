/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCBRKEMF_H_
#define _SCBRKEMF_H_


#include "SCEMFDefs.h"
#include "SCGenInclude.h"
#include SC_INC_GENLIB(SCBrkTarget.h)

// class used to parse metafile records
class SCBrkEMF : public SCBrkTarget
{
// Construction/Destruction
public:
	SCBrkEMF();
	virtual ~SCBrkEMF();
	void SCResetCracker();

// Accessors
#ifdef _DEBUG
    virtual CString SCGetClassName() const { return _T("SCBrkEMF"); };
#endif

	// getters
	HDC					SCGetEMFDC() const { return m_hPlayDC; }
	LPVOID				SCGetClientData() const { return m_pPlayData; }
	const RECT*			SCGetPlayRect() const { return m_lpPlayRect; }
	HANDLETABLE FAR *	SCGetHandleTable() const { return m_lpEnumHandleTable; }
	int					SCGetNbObj() const { return m_nEnumNbObjs; }
	void				SCGetRecordError(SC_BRKRESULT &CodeError,int &nRecordIndex) const {CodeError = m_ErrorRec; nRecordIndex = m_nCurRec;}
	BOOL				SCGetStop() const { return m_bStop; }

	// setters
	void				SCBrkSetThreshold(int nCbSpan) { m_nCbSpan = nCbSpan;}
	void				SCSetRecordError(SC_BRKRESULT CodeError) { m_ErrorRec = CodeError;}
	void				SCSetStop(BOOL bStop) { m_bStop = bStop; }

// Operation
public:
	SC_BRKRESULT SCBreakMetafile(HDC hdc, HENHMETAFILE hemf, LPVOID lpData, CONST RECT *lpRect);

// Implementation
protected:
	SC_BRKRESULT SCDispatchEMFRecord(ENHMETARECORD FAR *lpEnhMetaRecord, HANDLETABLE FAR *lpHandleTable);
	virtual long SCGetRecordSize() const;

	// helpers
		// document-oriented
	virtual SC_BRKRESULT SCBeginBreak(HDC hdc, LPVOID lpData, CONST RECT *lpRect);
	virtual void SCEndBreak();

		// record-oriented
	void SCPlayRecord();							// plays the current record
	void SCPlayRecord(void* pRec);					// plays the given record copy
	void SCPlayRecordAndFree(void* pRec);			// ditto, and free
	void SCPlayRecord(HDC hDC);						// plays the current record in the given DC
	void SCPlayRecord(HDC hDC, void* pRec);			// plays the given record copy in the given DC

	virtual SC_BRKRESULT SCBeginRecordBreak();		// called for per-record initialization
	virtual SC_BRKRESULT SCEndRecordBreak();		// called for per-record termination
	virtual SC_BRKRESULT OnBrkUNKRecord(long lMsg);

	// static
	static int CALLBACK sEnumEMFCallbak(HDC hDC,HANDLETABLE FAR *lpHandleTable, ENHMETARECORD FAR *lpEnhMetaRecord,	
							 int nObj, LPARAM lpData);
	
//mapped methods
//Add new EMF event handlers inside this block
//NOTE: The AFX prefix is used only for color stuff
	//{{AFX_SC_EMF_MSG
	virtual SC_BRKRESULT OnEmfHEADER();
	//}}AFX_SC_EMF_MSG
	//End of mapped messages
	SC_DECLARE_MESSAGE_MAP()

private:
//service methods

protected:
// data
	//got from outside
		//before enumeration
	HDC					m_hPlayDC;
	LPVOID				m_pPlayData;
	const RECT*			m_lpPlayRect;

		//after enumerating the first record
	HANDLETABLE FAR *	m_lpEnumHandleTable;
	int					m_nEnumNbObjs;

		//inside 
	BOOL				m_bStop;
	int					m_nCurRec;
	int					m_nCbSpan;
	SC_BRKRESULT		m_ErrorRec;
// specific
};

#endif //_SCBRKEMF_H_
//  ------------------------------------------------------------
