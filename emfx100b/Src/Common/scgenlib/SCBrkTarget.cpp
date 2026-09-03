/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCBrkTarget.h"

#ifdef _DEBUG
#	ifdef DEBUG_NEW
#		define new DEBUG_NEW
#		undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
#	endif
#endif

/////////////////////////////////////////////////////////////////////////////
// SCBrkTarget construction/destruction

SCBrkTarget::SCBrkTarget():
	m_pIBrkCallBack(NULL)
	, m_pRecord(NULL)
	, m_nLastError(SC_BRK_NOERROR)
#ifdef SC_USE_FLATMSGMAP
	, m_pFlatMsgMap(NULL)
#endif
{
}

SCBrkTarget::~SCBrkTarget()
{
#ifdef SC_USE_FLATMSGMAP
	if (m_pFlatMsgMap)
		delete [] m_pFlatMsgMap;
#endif
}

void* SCBrkTarget::SCCloneRecord() const
{
	if (!m_pRecord)
		return NULL;
	long lRecSize = SCGetRecordSize();

	if (!m_pRecord)
		return NULL;

	BYTE* pBytes = new BYTE[lRecSize];
	if (pBytes)
		memmove(pBytes, m_pRecord, lRecSize);

	return pBytes;
}


#ifdef SC_USE_FLATMSGMAP

SC_BRKRESULT SCBrkTarget::SCInitBreak()
{
	SCFlattenMessageMap();
	return SC_BRK_NOERROR;
}

// Solve inheritance once to speed up record dispatching
void SCBrkTarget::SCFlattenMessageMap()
{
	// Do it once
	if (m_pFlatMsgMap)
		return;

	// compute the maximum message index mapped for the final class
	const SC_MSGMAP* pMessageMap;
	const SC_MSGMAP_ENTRY* pEntry;
	m_lMaxMsg = 0;
	for (pMessageMap = SCGetMessageMap(); (pMessageMap != NULL);
	pMessageMap = pMessageMap->pBaseMap)
	{
		// Note: catches BEGIN_MESSAGE_MAP(class_A, class_A)
		ASSERT(pMessageMap != pMessageMap->pBaseMap);

		// Scan entries to find the code
		pEntry = pMessageMap->lpEntries;
		while(pEntry->nMsg != SCSig_end)
		{
			if (pEntry->nMsg > m_lMaxMsg)
				m_lMaxMsg = pEntry->nMsg;
			pEntry++;
		}
	}
	if (!m_lMaxMsg)
		return;

	m_lMaxMsg++; // messages are supposed to be one-based

	m_pFlatMsgMap = new PCSC_MSGMAP_ENTRY[m_lMaxMsg];
	memset(m_pFlatMsgMap, 0, m_lMaxMsg*sizeof(PSC_MSGMAP_ENTRY));

	// solve inheritance
	for (pMessageMap = SCGetMessageMap(); (pMessageMap != NULL);
	pMessageMap = pMessageMap->pBaseMap)
	{
		// Note: catches BEGIN_MESSAGE_MAP(CMyClass, CMyClass)!
		ASSERT(pMessageMap != pMessageMap->pBaseMap);

		// Scan entries to find the code
		pEntry = pMessageMap->lpEntries;
		while(pEntry->nMsg != SCSig_end)
		{
			if (NULL==m_pFlatMsgMap[pEntry->nMsg])
				m_pFlatMsgMap[pEntry->nMsg] = pEntry;
			pEntry++;
		}
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////
// SCBrkTarget record dispatching

#ifndef SC_USE_FLATMSGMAP
// code to use if the message map flattening is not used

// Message parsing
const SC_MSGMAP_ENTRY*
SCFindMessageEntry(const SC_MSGMAP_ENTRY* pEntry, long lMsg)
{
	// test entries
	if (!pEntry)
		return NULL;
	// Scan entries to find the code
	while (pEntry->nMsg != SCSig_end)
	{
		if (lMsg == pEntry->nMsg)
		{// Message found
			return pEntry;
		}
		pEntry++;
	}
	return NULL;
}

SC_BRKRESULT SCBrkTarget::SCInitBreak()
{
	return SC_BRK_NOERROR;
}

SC_BRKRESULT SCBrkTarget::OnBrkRecord(long lMsg)
{
	// determine the message number and code (packed into nCode)
	const SC_MSGMAP* pMessageMap;
	const SC_MSGMAP_ENTRY* pEntry;
	
	// look through message map to see if it applies to us
	for (pMessageMap = SCGetMessageMap(); (pMessageMap != NULL);
	pMessageMap = pMessageMap->pBaseMap)
	{
		// Note: catches BEGIN_MESSAGE_MAP(class_A, class_A)
		ASSERT(pMessageMap != pMessageMap->pBaseMap);
		
		pEntry = SCFindMessageEntry(pMessageMap->lpEntries, lMsg);
		if (pEntry != NULL)
		{
			// Handler found
			#ifdef _DEBUG
			TRACE2("SENDING record id 0x%04X to %hs target.\n", lMsg,
				SCGetClassName());
			#endif

			// Dispatch message
			return (this->*pEntry->pfn)();
		}
	}
	// Dispatch unknown (unmapped) record
	return OnBrkUNKRecord(lMsg);   // record not handled
}

#else

// message map is flat
inline const SC_MSGMAP_ENTRY* SCBrkTarget::SCFindMessageEntry(long lMsg)
{
	ASSERT(m_pFlatMsgMap);
	// messages are supposed to be one-based
	if (lMsg>0 && lMsg<m_lMaxMsg)
		return m_pFlatMsgMap[lMsg];

	return NULL;
}

SC_BRKRESULT SCBrkTarget::OnBrkRecord(long lMsg)
{
	const SC_MSGMAP_ENTRY* pEntry = SCFindMessageEntry(lMsg);
	if (pEntry != NULL)
	{
		// Handler found
#if 0
		#ifdef _DEBUG
			TRACE2("SENDING record id 0x%04X to %hs target.\n", lMsg,
				SCGetClassName());
		#endif
#endif
		// Dispatch message
		return (this->*pEntry->pfn)();
	}

	// Dispatch unknown (unmapped) record
	return OnBrkUNKRecord(lMsg);   // record not handled
}

#endif

// Derived classes should implement this to handle unmapped records
SC_BRKRESULT SCBrkTarget::OnBrkUNKRecord(long lMsg)
{
	return SC_BRK_RECORDHANDLER_NOTFOUND; // not handled
}

/////////////////////////////////////////////////////////////////////////////
// Root of message maps

const SC_MSGMAP SCBrkTarget::m_messageMap =
{
	NULL,
	&SCBrkTarget::m_messageEntries[0]
};

const SC_MSGMAP* SCBrkTarget::SCGetMessageMap() const
{
	return &SCBrkTarget::m_messageMap;
}

const SC_MSGMAP_ENTRY SCBrkTarget::m_messageEntries[] =
{
	{SCSig_end, 0 }     // nothing here
};

