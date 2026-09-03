/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCREFOBJ_H_
#define _SCREFOBJ_H_

///
///	Little class to keep the counter
///
class SCCounterKeeper
{
public:
	SCCounterKeeper() : m_ulCounter(0)	{;}
	unsigned long &SCGet()				{ return m_ulCounter; }
private:
	unsigned long m_ulCounter;
};


///
///	Declare SCAddRef/SCRelease and the counter
///
#define SC_DECLARE_REFCOUNT() \
public : \
    unsigned long SCAddRef();\
    unsigned long SCRelease();\
    unsigned long SCGetRefCount() { return _m_SCCounter.SCGet();}\
protected:\
    SCCounterKeeper _m_SCCounter;


///
/// Implement (SCAddRef/SCRelease)
///
#define SC_IMPLEMENT_REFCOUNT(ClassName)\
unsigned long ClassName##::SCAddRef()\
{\
    return ++(_m_SCCounter.SCGet());\
}\
unsigned long ClassName##::SCRelease()\
{\
    if (--(_m_SCCounter.SCGet())) return (_m_SCCounter.SCGet()); \
    delete this;\
	return 0;\
}

///
/// Implement (SCAddRef/SCRelease) for template
///
#define SC_IMPLEMENT_REFCOUNT_TEMPLATE(ClassName)\
template <class T> \
unsigned long ClassName##< T >::SCAddRef()\
{\
    return ++(_m_SCCounter.SCGet());\
}\
template <class T> \
unsigned long ClassName##< T >::SCRelease()\
{\
    if (--(_m_SCCounter.SCGet())) return (_m_SCCounter.SCGet()); \
    delete this;\
	return 0;\
}

///
///	Operation macros
///
#define SC_REFCOUNTED_ADDREF(pObj)\
	((pObj) ?  pObj->SCAddRef() : 0)

#define SC_REFCOUNTED_RELEASE(pObj)\
	((pObj) ?  pObj->SCRelease() : 0)

#define SC_REFCOUNTED_SAFERELEASE(pObj)\
	((pObj)? pObj=(0==pObj->SCRelease())?NULL:pObj :0)


#define SC_REFCOUNTED_AUTO_ADDREF()\
	this->SCAddRef()

#define SC_REFCOUNTED_AUTO_RELEASE()\
	this->SCRelease()

#endif //_SCREFOBJ_H_
//  ------------------------------------------------------------
