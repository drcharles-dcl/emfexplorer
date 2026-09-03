/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCBRKTARGET_H_
#define _SCBRKTARGET_H_

#include "SCBrkDefs.h"
#include "SCGenTypes.h"

// Interface to be implemented by a callback to manage record breaking events
class I_SCBrkCallBack
{
public:
	virtual long OnSCBrkPos(long lCurPos)=0;
	virtual long OnSCBrkMaxPos(long lMaxPos)=0;
	virtual long OnSCBrkRecStr(long lCurPos, SCgenTCHAR *szRecStr)=0;
};

class SCBrkTarget
{
// Constructor/Destructor
public:
	SCBrkTarget();
	virtual ~SCBrkTarget();

// Methods
public:
	SC_BRKRESULT OnBrkRecord(long lMsg);
	virtual SC_BRKRESULT OnBrkUNKRecord(long lMsg);

//accessors
	// getters
#ifdef _DEBUG
    virtual CString SCGetClassName() const { return _T("SCBrkTarget"); };
#endif

	SC_BRKRESULT SCGetLastError() const { return m_nLastError; }
	void* SCGetLastRecord() const { return m_pRecord; }

	// setters
	void SCSetBrkCallBack(I_SCBrkCallBack *pBrkCallBack) {m_pIBrkCallBack=pBrkCallBack;}

protected:
	virtual long SCGetRecordSize() const { return 0; }
	void* SCCloneRecord() const;
	void SCFreeCloneRecord(void* pCloneRec) const { ASSERT(pCloneRec); delete [] pCloneRec; }
	void SCSetError(SC_BRKRESULT nError) { m_nLastError = nError; }
	SC_BRKRESULT SCInitBreak();

	SC_DECLARE_MESSAGE_MAP()       // base class - no {{ }} macros
#ifdef SC_USE_FLATMSGMAP
	void				SCFlattenMessageMap();
#endif

//shared
protected:
	I_SCBrkCallBack*	m_pIBrkCallBack; // installed callback
	void*				m_pRecord;		 // current record
	SC_BRKRESULT		m_nLastError;	 // error while parsing last record

//data
private:
#ifdef SC_USE_FLATMSGMAP
	PCSC_MSGMAP_ENTRY*	m_pFlatMsgMap;   // message map entries with inheritance solved
	long				m_lMaxMsg;
	inline const SC_MSGMAP_ENTRY* SCFindMessageEntry(long lMsg);
#endif

//prohibited calls
private:
 /*No default =*/void operator=(const SCBrkTarget &rCopySCBrkTarget) { ; }
 /*No default copy*/SCBrkTarget(const SCBrkTarget &rCopySCBrkTarget) { ; }
};

#endif //_SCBRKTARGET_H_
//  ------------------------------------------------------------
