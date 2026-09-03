/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCREFDOBJHOLDER_T_H_
#define _SCREFDOBJHOLDER_T_H_

#include "SCRefObj.h"

// Comment out this line to optimize for size
#define SC_SPEED_REFCOUNT

////////////////////////////////////////////////////////////////////////////////////////
//
//	SCRefdObjHolder
//
////////////////////////////////////////////////////////////////////////////////////////

///
/// Class to perfom external reference counting.
/// It holds a pointer that is deleted in its destructor.
///
template <class T>
class SCRefdObjHolder 
{
// Construction / Destruction
public:
	SCRefdObjHolder(T* pObj):
	  m_pObj(pObj)
	{
		ASSERT(pObj);
		SC_REFCOUNTED_AUTO_ADDREF();
	}
	  
	virtual ~SCRefdObjHolder()
	{
		// Ensure that this object is deleted by SC_REFCOUNTED_RELEASE,
		// not by a direct call to delete.
		ASSERT(0==SCGetRefCount());
		if (m_pObj)
			delete (T*) m_pObj; // safe delete
	}

// Operation
#ifndef SC_SPEED_REFCOUNT

	// Restore a pair holder/object from a source holder
	static void SCObjHolderRestore(SCRefdObjHolder< T >*& pHolder, T*& pObj, SCRefdObjHolder< T >* pSvdHolder)
	{
		SC_REFCOUNTED_RELEASE(pHolder);		// delete the current one if possible
		pHolder = pSvdHolder;
		if (pHolder)
		{
			SC_REFCOUNTED_ADDREF(pHolder);	// lock the new one, since it may be the same as the current one
			pObj = pHolder->SCGetObj();
		} else
			pObj = NULL;
	}

	// Release the given holder and set pointers to NULL to generate exception in case of
	// reuse without reallocation.
	// (The macro SC_OBJHOLDER_RELEASE_T, defined below, is preferred)
	static void SCObjHolderRelease(SCRefdObjHolder< T >*& pHolder, T*& pObj)
	{
		SC_REFCOUNTED_RELEASE(pHolder); // will not null out pHolder
		pHolder = NULL;
		pObj = NULL;
	}

	// Release the given holder, create a new one and bind it to an object
	//
	// (The macro SC_OBJHOLDER_RENEW_T, defined below, is preferred)
	static void SCObjHolderRenew(SCRefdObjHolder< T >*& pHolder, T* pObj)
	{
		SC_REFCOUNTED_RELEASE(pHolder);
		if (pObj)
			pHolder = new SCRefdObjHolder< T >(pObj);
		else
			pHolder = NULL;
	}
#endif

// Accessors
	// getters
	T* SCGetObj() const { return m_pObj; }
	T* SCDetachObj()	{ T* pObj=m_pObj; m_pObj = NULL; return pObj; }

// Implementation
	SC_DECLARE_REFCOUNT();

// Data
private:
	T* m_pObj;

//prohibited calls
private:
	SCRefdObjHolder();	// prevent creation on the stack
	/*No default =*/void operator=(const SCRefdObjHolder &rCopy);
	/*No default copy*/SCRefdObjHolder(const SCRefdObjHolder &rCopy);
};

///
/// Implement (SCAddRef/SCRelease)
///
SC_IMPLEMENT_REFCOUNT_TEMPLATE(SCRefdObjHolder)

///////////////////////////////////////////////////////////////////////////////////////
/// Macros to manage a pair of ObjectHolder/Object
///

#ifdef SC_SPEED_REFCOUNT

///
/// Restore a pair holder/object from a source holder.
/// (see SCObjHolderRestore for documentation)
///
#define SC_OBJHOLDER_RESTORE_T(pHolder, pObj, pSvdHolder, unused1) \
	SC_REFCOUNTED_RELEASE(pHolder),								\
	pHolder = pSvdHolder,										\
	(pHolder)?													\
		SC_REFCOUNTED_ADDREF(pHolder),							\
		pObj = pHolder->SCGetObj()								\
	:															\
		pObj = NULL

///
/// Release the given holder and set pointers to NULL to generate exception in case of
/// reuse without reallocation.
///
	// We lose here by one redundant parameter
	// ('type' can be omitted, causing warning C4003)
#define SC_OBJHOLDER_RELEASE_T(pHolder, pObj, unused1)	\
	SC_REFCOUNTED_RELEASE(pHolder),						\
	pHolder = NULL,										\
	pObj = NULL

///
/// Release the given holder, create a new one and bind it to an object
/// (type is required)
///
#define SC_OBJHOLDER_RENEW_T(pHolder, pObj, type)			\
	SC_REFCOUNTED_RELEASE(pHolder),							\
	(pObj) ? pHolder = new SCRefdObjHolder< ##type >(pObj)	\
		   : pHolder = NULL

#else
	// Here, type is fully required

#define SC_OBJHOLDER_RESTORE_T(pHolder, pObj, pSvdHolder, type) \
	SCRefdObjHolder<##type>::SCObjHolderRestore(pHolder, pObj, pSvdHolder)

#define SC_OBJHOLDER_RELEASE_T(pHolder, pObj, type) \
	SCRefdObjHolder<##type>::SCObjHolderRelease(pHolder, pObj)

#define SC_OBJHOLDER_RENEW_T(pHolder, pObj, type) \
	SCRefdObjHolder<##type>::SCObjHolderRenew(pHolder, pObj)

#endif

/////////////////////////////////////////////////////////
///
#define SC_OBJHOLDER_SAFERELEASE_T(pHolder, pObj, type)\
	SC_REFCOUNTED_SAFERELEASE(pHolder),\
		pObj = (!pHolder) ? NULL : pObj


////////////////////////////////////////////////////////////////////////////////////////
//
//	SCRefdObjContainer
//
////////////////////////////////////////////////////////////////////////////////////////

///
/// Class to perfom external reference counting,
///  and serve as a container for subobject S.
///
template <class T, class S>
class SCRefdObjContainer : public SCRefdObjHolder<T>
{
typedef SCRefdObjHolder<T> SCClassT; // compiler won't understand direct 'wording'
// Construction / Destruction
public:
	SCRefdObjContainer(T* pObjT, S* pObjS):
		SCClassT(pObjT),
		m_pSubObj(pObjS)
	{
	}
	  
	virtual ~SCRefdObjContainer()
	{
		// Ensure that this object is deleted by SC_REFCOUNTED_RELEASE,
		// not by a direct call to delete.
		ASSERT(0==SCGetRefCount());
		if (m_pSubObj)
			delete (S*) m_pSubObj; // safe delete
	}

// Operation
#ifndef SC_SPEED_REFCOUNT
	typedef SCRefdObjContainer<T,S> SCHolderT;

	static void SCObjHolderRestore(SCHolderT*& pHolder, T*& pObj, SCHolderT* pSvdHolder)
	{
		SC_REFCOUNTED_RELEASE(pHolder);
		pHolder = pSvdHolder;
		if (pHolder)
		{
			SC_REFCOUNTED_ADDREF(pHolder);
			pObj = pHolder->SCGetObj();
		} else
			pObj = NULL;
	}

	static void SCObjHolderRelease(SCHolderT*& pHolder, T*& pObj)
	{
		SC_REFCOUNTED_RELEASE(pHolder);
		pHolder = NULL;
		pObj = NULL;
	}

	static void SCObjHolderRenew(SCHolderT*& pHolder, T* pObj, S* pSObj)
	{
		SC_REFCOUNTED_RELEASE(pHolder);
		if (pObj)
			pHolder = new SCHolderT(pObj, pSObj);
		else
			pHolder = NULL;
	}
#endif

// Accessors
	// getters
	S* SCGetSubObj() const { return m_pSubObj; }
	S* SCDetachSubObj()	   { S* pObj=m_pSubObj; m_pSubObj = NULL; return pObj; }

// Implementation
	//SC_DECLARE_REFCOUNT(); // done in base template

// Data
private:
	S* m_pSubObj;

//prohibited calls
private:
	SCRefdObjContainer();	// prevent creation on the stack
	/*No default =*/void operator=(const SCRefdObjContainer &rCopy);
	/*No default copy*/SCRefdObjContainer(const SCRefdObjContainer &rCopy);
};

///
/// Implement (SCAddRef/SCRelease)
///	(done in base template)
//	SC_IMPLEMENT_REFCOUNT_TEMPLATE(SCRefdObjContainer)

///////////////////////////////////////////////////////////////////////////////////////
/// Macros to manage a pair of ObjectContainer/Object
///

#ifdef SC_SPEED_REFCOUNT

#define SC_OBJCONTAINER_RESTORE_T(pHolder, pObj, pSvdHolder, unused1, unused2)	\
	SC_REFCOUNTED_RELEASE(pHolder),								\
	pHolder = pSvdHolder,										\
	(pHolder)?													\
		SC_REFCOUNTED_ADDREF(pHolder),							\
		pObj = pHolder->SCGetObj()								\
	:															\
		pObj = NULL

#define SC_OBJCONTAINER_RELEASE_T(pHolder, pObj, type, unused1) \
	SC_OBJHOLDER_RELEASE_T(pHolder, pObj, type)

#define SC_OBJCONTAINER_RENEW_T(pHolder, pObj, type, pSObj, subtype)				\
	SC_REFCOUNTED_RELEASE(pHolder),													\
	(pObj)	?	pHolder = new SCRefdObjContainer< ##type, ##subtype >(pObj, pSObj)	\
			:	pHolder = NULL

#else

#define SC_OBJCONTAINER_RESTORE_T(pHolder, pObj, pSvdHolder, type, subtype) \
	SCRefdObjContainer<##type, ##subtype>::SCObjHolderRestore(pHolder, pObj, pSvdHolder)

#define SC_OBJCONTAINER_RELEASE_T(pHolder, pObj, type, subtype) \
	SCRefdObjContainer<##type, ##subtype>::SCObjHolderRelease(pHolder, pObj)

#define SC_OBJCONTAINER_RENEW_T(pHolder, pObj, type, pSObj, subtype) \
	SCRefdObjContainer<##type, ##subtype>::SCObjHolderRenew(pHolder, pObj, pSObj)

#endif

#endif //_SCREFDOBJHOLDER_T_H_
//  ------------------------------------------------------------
