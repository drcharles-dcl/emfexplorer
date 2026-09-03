/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCBRKDEFS_H_
#define _SCBRKDEFS_H_


typedef long SC_BRKRESULT;
		
#define SC_BRK_NOERROR					0
#define SC_BRK_STOPBREAKING				1
#define SC_BRK_UNEXPECTED				2
#define SC_BRK_BADDATASOURCE			3
#define SC_BRK_RECORDHANDLER_NOTFOUND	4
#define SC_BRK_ENUMERATIONFAILED		5

// Message map management
#define SCSig_end	-1L
#define SC_USE_FLATMSGMAP

// ensure one byte packing
#pragma pack(1)

class SCBrkTarget;
typedef SC_BRKRESULT (SCBrkTarget::*fnEventHandler)();

#define ON_VECTRECORD(id, memberFxn) \
	{ (long)id, (fnEventHandler)&memberFxn },

#define SC_PMSG fnEventHandler

typedef struct tag_SC_MSGMAP_ENTRY
{
	long	nMsg;   // record type
	SC_PMSG pfn;    // routine to call
} SC_MSGMAP_ENTRY;
typedef SC_MSGMAP_ENTRY *PSC_MSGMAP_ENTRY;
typedef const SC_MSGMAP_ENTRY *PCSC_MSGMAP_ENTRY;

typedef struct tag_SC_MSGMAP
{
	const tag_SC_MSGMAP* pBaseMap;
	const SC_MSGMAP_ENTRY* lpEntries;
} SC_MSGMAP;
typedef SC_MSGMAP *PSC_MSGMAP;

#define SC_DECLARE_MESSAGE_MAP() \
private: \
	static const SC_MSGMAP_ENTRY m_messageEntries[]; \
protected: \
	static const SC_MSGMAP m_messageMap; \
	virtual const SC_MSGMAP* SCGetMessageMap() const; \


#define SC_BEGIN_MESSAGE_MAP(theClass, baseClass) \
	const SC_MSGMAP* theClass::SCGetMessageMap() const \
		{ return &theClass::m_messageMap; } \
	const SC_MSGMAP theClass::m_messageMap = \
	{ &baseClass::m_messageMap, &theClass::m_messageEntries[0] }; \
	const SC_MSGMAP_ENTRY theClass::m_messageEntries[] = \
	{ \


#define SC_END_MESSAGE_MAP() \
		{SCSig_end, 0L } \
	}; \

// revert to default packing
#pragma pack()

#endif //_SCBRKDEFS_H_
//  ------------------------------------------------------------
