/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCBrkEMF.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SCBrkEMF::SCBrkEMF():
	SCBrkTarget(),
	//got from outside
	m_hPlayDC(NULL),
	m_pPlayData(NULL),
	m_lpPlayRect(NULL),
	m_lpEnumHandleTable(NULL),
	m_nEnumNbObjs(0),
	//inside
	m_nCurRec(0),
	m_nCbSpan(0),
	m_ErrorRec(0),
	m_bStop(FALSE)
{
}

SCBrkEMF::~SCBrkEMF()
{
}

void SCBrkEMF::SCResetCracker()
{
}


//////////////////////////////////////////////////////////////////////
// Events methods
//////////////////////////////////////////////////////////////////////
SC_BRKRESULT SCBrkEMF::OnEmfHEADER()
{
	// TRACE0("**EMR_HEADER\n");

	// DONE_EMF: Place code here to handle EMF record
	if (m_pIBrkCallBack)
	{
		ASSERT(m_pRecord);
		long lRes = m_pIBrkCallBack->OnSCBrkMaxPos(((ENHMETAHEADER*)m_pRecord)->nRecords);
		if (0==lRes)
			return SC_BRK_STOPBREAKING;
	}
	
	return SC_BRK_NOERROR;
}

// Derived classes should implement this to handle unmapped records
SC_BRKRESULT SCBrkEMF::OnBrkUNKRecord(long lMsg)
{
	// TRACE0("**OnBrkUNKRecord\n");
	SCPlayRecord(); // by default, let Windows manage it
	return SC_BRK_NOERROR;
}


//////////////////////////////////////////////////////////////////////
// Events dispatcher
//////////////////////////////////////////////////////////////////////
SC_BRKRESULT SCBrkEMF::SCDispatchEMFRecord(ENHMETARECORD FAR *lpEnhMetaRecord, HANDLETABLE FAR *lpHandleTable)	
{
	ASSERT(lpEnhMetaRecord);
	if (m_bStop)
		return SC_BRK_STOPBREAKING;

	// private initialization
	m_pRecord = lpEnhMetaRecord;
	m_lpEnumHandleTable = lpHandleTable;

#if 1
	// Record-level control needed (m_pIBrkCallBack!=NULL)

	// TRACE0("\nbegin SCBrkEMF::SCDispatchEMFRecord\n");
	SC_BRKRESULT error(SC_BRK_NOERROR);

	// Prolog:
	// if per-record initialization fails, stop metafile enumeration
	if (error=SCBeginRecordBreak())
		return error;
	
	// Dispatch:
	if (error = this->OnBrkRecord((long)lpEnhMetaRecord->iType))
		return error;
	
	// Epilog:
	// if per-record termination fails, stop metafile enumeration
	error = SCEndRecordBreak();
	
	// TRACE0("end SCBrkEMF::SCDispatchEMFRecord\n");
	return error;
#else
	// speed needed
	return this->OnBrkRecord((long)lpEnhMetaRecord->iType);
#endif
}

// Implementation
//

long SCBrkEMF::SCGetRecordSize() const
{
	if (m_pRecord)
		return ((EMR*)m_pRecord)->nSize;
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Starter methods
//////////////////////////////////////////////////////////////////////
int CALLBACK SCBrkEMF::sEnumEMFCallbak(HDC hDC,HANDLETABLE FAR *lpHandleTable, ENHMETARECORD FAR *lpEnhMetaRecord,	
							 int nObj, LPARAM lpData)
{
	SC_BRKRESULT error(SC_BRK_NOERROR);
	SCBrkEMF* pBrkTarget = (SCBrkEMF*)lpData;

	// nObj was stored before enumeration started

	ASSERT(pBrkTarget);
	if (error = pBrkTarget->SCDispatchEMFRecord(lpEnhMetaRecord, lpHandleTable))
	{
		pBrkTarget->SCSetRecordError(error);
		return 0;
	}

	return 1;
}

SC_BRKRESULT SCBrkEMF::SCBreakMetafile(HDC hdc, HENHMETAFILE hemf, LPVOID lpData,
										 CONST RECT *lpRect)
{
	ASSERT(hemf);

	// Setup permanent app data
	SC_BRKRESULT error = SCBeginBreak(hdc, lpData, lpRect);

	// Enumerate
	if (SC_BRK_NOERROR == error)
	{
		ENHMETAHEADER EmfHeader;
		if (::GetEnhMetaFileHeader(hemf, sizeof(ENHMETAHEADER), &EmfHeader))
		{
			m_nEnumNbObjs = EmfHeader.nHandles;
		} else
			return SC_BRK_BADDATASOURCE;
		
		int nEmumRes = ::EnumEnhMetaFile(m_hPlayDC, hemf, (ENHMFENUMPROC)SCBrkEMF::sEnumEMFCallbak, (void*)this, lpRect);
		
		SCEndBreak();

		if (!nEmumRes)
		{// error !?
			// We should call GetLastError. But most of the time it returns 0 (meaning "no error").
			error = m_ErrorRec = SC_BRK_ENUMERATIONFAILED;
		}
	}

	return error;
}

//////////////////////////////////////////////////////////////////////
// Helpers
//////////////////////////////////////////////////////////////////////

SC_BRKRESULT SCBrkEMF::SCBeginBreak(HDC hdc, LPVOID lpData, CONST RECT *lpRect)
{
	SC_BRKRESULT error = SCInitBreak();

	//got from outside
	m_hPlayDC = hdc;
	m_pPlayData = lpData;
	m_lpPlayRect = lpRect;
	m_lpEnumHandleTable = NULL;
	m_nEnumNbObjs = 0;

	//inside
	m_pRecord = NULL;
	m_nCurRec = 0;
	m_bStop = FALSE;
	return error;
}

void SCBrkEMF::SCEndBreak()
{
	if (!m_bStop && m_pIBrkCallBack) 	
		m_pIBrkCallBack->OnSCBrkPos(m_nCurRec);
}

// called for per-record initialization
SC_BRKRESULT SCBrkEMF::SCBeginRecordBreak()
{
	if (m_bStop)
		return SC_BRK_STOPBREAKING;

	m_nCurRec++;
	return SC_BRK_NOERROR;
}

// play the current record
void SCBrkEMF::SCPlayRecord()
{
	ASSERT(m_pRecord);
	::PlayEnhMetaFileRecord(m_hPlayDC, m_lpEnumHandleTable, (ENHMETARECORD*)m_pRecord, m_nEnumNbObjs);
}

// play a given record (generally a copy)
void SCBrkEMF::SCPlayRecord(void* pRec)
{
	ASSERT(pRec);
	::PlayEnhMetaFileRecord(m_hPlayDC, m_lpEnumHandleTable, (ENHMETARECORD*)pRec, m_nEnumNbObjs);
}

// play the current record in another DC
void SCBrkEMF::SCPlayRecord(HDC hDC)
{
	ASSERT(m_pRecord);
	ASSERT(hDC);
	::PlayEnhMetaFileRecord(hDC, m_lpEnumHandleTable, (ENHMETARECORD*)m_pRecord, m_nEnumNbObjs);
}

// play a given record record in another DC
void SCBrkEMF::SCPlayRecord(HDC hDC, void* pRec)
{
	ASSERT(pRec);
	ASSERT(hDC);
	::PlayEnhMetaFileRecord(hDC, m_lpEnumHandleTable, (ENHMETARECORD*)pRec, m_nEnumNbObjs);
}


// play a given record (generally a copy), and free memory
void SCBrkEMF::SCPlayRecordAndFree(void* pRec)
{
	ASSERT(pRec);
	::PlayEnhMetaFileRecord(m_hPlayDC, m_lpEnumHandleTable, (ENHMETARECORD*)pRec, m_nEnumNbObjs);
	SCFreeCloneRecord(pRec);
}


// called for per-record termination
SC_BRKRESULT SCBrkEMF::SCEndRecordBreak()
{
	SC_BRKRESULT error(SC_BRK_NOERROR);
	if (m_bStop)
		return SC_BRK_STOPBREAKING;

	if (m_pIBrkCallBack && m_nCbSpan && !(m_nCurRec % m_nCbSpan)) 
	{	
		long lRes = m_pIBrkCallBack->OnSCBrkPos(m_nCurRec);
		if (0==lRes)
			return SC_BRK_STOPBREAKING;
	}
	return error;
}


//////////////////////////////////////////////////////////////////////
// Message map
//////////////////////////////////////////////////////////////////////
SC_BEGIN_MESSAGE_MAP(SCBrkEMF, SCBrkTarget)
ON_VECTRECORD(SCEMF_HEADER, OnEmfHEADER)
SC_END_MESSAGE_MAP()

